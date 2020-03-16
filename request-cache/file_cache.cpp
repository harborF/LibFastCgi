#include "settings.h"

#include <unistd.h>
#include <cerrno>
#include <stdexcept>

#include "fastcgi2-ext/ComponentContext.h"
#include "fastcgi2/ComponentFactory.h"
#include "fastcgi2/config.h"
#include "fastcgi2/DataBuffer.h"
#include "fastcgi2/logger.h"
#include "fastcgi2/FastcgiUtils.h"

#include "file_buffer.h"
#include "file_cache.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    struct KeyId {
        KeyId() : id(0), last(time(NULL)) {}
        std::string get() {
            time_t now = time(NULL);
            if (now == last) {
                ++id;
            }
            else {
                last = now;
                id = 0;
            }
            std::string result;
            result.append((char*)&id, sizeof(id));
            result.push_back(':');
            result.append((char*)&last, sizeof(last));
            result.push_back(':');
            std::thread::id thread_id = std::this_thread::get_id();
            result.append((char*)&thread_id, sizeof(thread_id));
            return HashUtils::hexMD5(result.c_str(), result.size());
        }

        uint32_t id;
        time_t last;
    };

    static thread_local KeyId* key_id_holder;

    std::string FileRequestCache::generateUniqueKey() {
        if (NULL == key_id_holder) {
            key_id_holder = new KeyId();
        }
        return key_id_holder->get();
    }

    class RequestCacheStream : public RequestIOStream {
    public:
        int read(char *buf, int size) {
            (void)buf;
            return size;
        }
        int write(const char *buf, int size) {
            (void)buf;
            return size;
        }
        void write(std::streambuf *buf) {
            (void)buf;
        }
    };

    FileRequestCache::FileRequestCache(ComponentContext *context) :
        Component(context), globals_(NULL), logger_(NULL), stopped_(false) {

        ComponentContextImpl* impl = dynamic_cast<ComponentContextImpl*>(context);
        if (NULL == impl) {
            throw std::runtime_error("cannot fetch globals in request cache");
        }
        globals_ = impl->globals();

        const Config *config = context->getConfig();
        const std::string componentXPath = context->getComponentXPath();

        cache_dir_ = config->asString(
            componentXPath + "/cache-dir", "/var/cache/fastcgi-daemon/request-cache/");

        if (cache_dir_.empty()) {
            throw std::runtime_error("Empty cache directory");
        }

        if (*cache_dir_.rbegin() != '/') {
            cache_dir_.push_back('/');
        }

        window_ = config->asInt(componentXPath + "/file-window", 1024 * 1024);
        max_retries_ = config->asInt(componentXPath + "/max-retries", 2);
        min_post_size_ = config->asInt(componentXPath + "/min-post-size", 1024 * 1024);
    }

    FileRequestCache::~FileRequestCache() {
    }

    void FileRequestCache::onLoad() {
        std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
        logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
        if (!logger_) {
            throw std::runtime_error("cannot get component " + loggerComponentName);
        }

        thread_.reset(new std::thread(std::mem_fn(&FileRequestCache::handle), this));
    }

    void FileRequestCache::onUnload() {
        stop();
    }

    DataBuffer FileRequestCache::createFileBuffer(const std::string &key) {
        std::string path = cache_dir_ + key;
        return DataBuffer::create(new FileBuffer(path.c_str(), window_));
    }

    DataBuffer FileRequestCache::create() {
        std::string key = generateUniqueKey();
        DataBuffer buffer = createFileBuffer(key);
        if (!buffer.isNil()) {
            std::unique_lock<std::mutex> lock(active_mutex_);
            active_.insert(std::make_pair(key, 0));
        }
        return buffer;
    }

    std::string FileRequestCache::getStoredKey(Request *request) {
        DataBuffer request_buffer = request->requestBody();
        FileBuffer* impl = dynamic_cast<FileBuffer*>(request_buffer.impl());
        const std::string& filename = impl ? impl->filename() : StringUtils::EMPTY_STRING;
        if (!filename.empty() &&
            0 == strncmp(filename.c_str(), cache_dir_.c_str(), cache_dir_.size())) {
            return filename.substr(cache_dir_.size());
        }
        return StringUtils::EMPTY_STRING;
    }

    std::string FileRequestCache::getKey(Request *request) {
        std::string key = getStoredKey(request);
        return key.empty() ? generateUniqueKey() : key;
    }

    std::string FileRequestCache::createHardLink(const std::string &key) {
        std::string path = cache_dir_ + key;
        std::string new_key = generateUniqueKey();
        std::string new_path = cache_dir_ + new_key;
        if (-1 == link(path.c_str(), new_path.c_str())) {
            char buffer[256];
            logger_->error("Cannot link file %s: %s", path.c_str(), strerror_r(errno, buffer, sizeof(buffer)));
            return StringUtils::EMPTY_STRING;
        }
        return new_key;
    }

    bool FileRequestCache::saveRequest(Request *request, const std::string &key, std::string &new_key) {
        FileBuffer* impl = dynamic_cast<FileBuffer*>(request->requestBody().impl());
        const std::string& filename = impl ? impl->filename() : StringUtils::EMPTY_STRING;
        DataBuffer buffer;
        if (filename.empty() ||
            0 != strncmp(filename.c_str(), cache_dir_.c_str(), cache_dir_.size())) {
            buffer = createFileBuffer(key);
            if (buffer.isNil()) {
                return false;
            }
            request->serialize(buffer);
        }
        new_key = createHardLink(key);
        return !new_key.empty();
    }

    void FileRequestCache::eraseActive(const std::string &key) {
        std::unique_lock<std::mutex> lock(active_mutex_);
        active_.erase(key);
    }

    void FileRequestCache::insertWaiting(time_t delay, const std::string &key, int retries) {
        std::unique_lock<std::mutex> lock(waiting_mutex_);
        waiting_.insert(std::make_pair(delay + time(NULL), DelayTask(key, retries)));
    }

    void FileRequestCache::save(Request *request, time_t delay) {

        if (0 == delay || max_retries_ <= 0) {
            std::string key = getStoredKey(request);
            if (!key.empty()) {
                eraseActive(key);
            }
            return;
        }

        std::string key = getKey(request);
        unsigned int retries = 0;
        bool active_found = false;
        {
            std::unique_lock<std::mutex> lock(active_mutex_);
            std::map<std::string, int>::iterator it = active_.find(key);
            if (active_.end() != it) {
                active_found = true;
                retries = it->second;
            }
        }

        if (!active_found) {
            std::string new_key;
            if (!saveRequest(request, key, new_key)) {
                logger_->error("Cannot save request for %s", request->getScriptName().c_str());
                return;
            }
            insertWaiting(delay, new_key, 1);
            return;
        }

        if (retries >= max_retries_) {
            logger_->info("Max retries reach for %s", request->getScriptName().c_str());
            eraseActive(key);
            return;
        }


        std::string new_key;
        if (!saveRequest(request, key, new_key)) {
            logger_->error("Cannot save request for %s", request->getScriptName().c_str());
            eraseActive(key);
            return;
        }

        eraseActive(key);
        insertWaiting(delay, new_key, retries + 1);
    }

    uint32_t FileRequestCache::minPostSize() const {
        return min_post_size_;
    }

    void FileRequestCache::stop() {
        stopped_ = true;
        {
            std::unique_lock<std::mutex> l(mutex_);
            condition_.notify_all();
        }
        thread_->join();
    }

    void FileRequestCache::handle() {
        while (true) {
            try {
                if (stopped_) {
                    return;
                }

                DelayTask delay_task;
                {
                    std::unique_lock<std::mutex> lock(waiting_mutex_);
                    std::multimap<time_t, DelayTask>::iterator task_it = waiting_.begin();
                    if (waiting_.end() == task_it || task_it->first > time(NULL)) {
                        lock.unlock();

                        auto t = std::chrono::system_clock::now() + std::chrono::seconds(1);
                        std::unique_lock<std::mutex> l(mutex_);
                        condition_.wait_until(l, t);
                        continue;
                    }
                    delay_task = task_it->second;
                    waiting_.erase(task_it);
                }

                RequestTask task;
                task.request = std::shared_ptr<Request>(new Request(logger_, this));
                task.request_stream = std::shared_ptr<RequestIOStream>(new RequestCacheStream());

                DataBuffer buffer = createFileBuffer(delay_task.key);
                if (buffer.isNil()) {
                    logger_->error("Cannot load file %s", delay_task.key.c_str());
                }
                else {
                    std::unique_lock<std::mutex> lock(active_mutex_);
                    active_.insert(std::make_pair(delay_task.key, delay_task.retries));
                }
                task.request->parse(buffer);
                handleRequest(task);
            }
            catch (const std::exception &e) {
                logger_->error("caught exception while handling request: %s", e.what());
            }
            catch (...) {
                logger_->error("caught unknown exception while handling request");
            }
        }
    }

    const Globals* FileRequestCache::globals() const {
        return globals_;
    }

    Logger* FileRequestCache::logger() const {
        return logger_;
    }

} //namespace fastcgi

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("request-cache", fastcgi::FileRequestCache)
FCGIDAEMON_REGISTER_FACTORIES_END()
