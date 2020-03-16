#ifndef _FASTCGI_REQUEST_CACHE_FILE_BUFFER_H_
#define _FASTCGI_REQUEST_CACHE_FILE_BUFFER_H_

#include <string>
#include <vector>
#include <mutex>

#include "mmap_file.h"
#include "fastcgi2-ext/DataBufferImpl.h"

namespace fastcgi
{
    struct FileHolder {
        std::string filename;
        FileHolder(const std::string &name) : filename(name) {}
        ~FileHolder() {
            unlink(filename.c_str());
        }
    };

    class FileBuffer : public DataBufferImpl {
    public:
        FileBuffer(const char *name, uint64_t window);
        virtual ~FileBuffer();
        virtual uint64_t read(uint64_t pos, char *data, uint64_t len);
        virtual uint64_t write(uint64_t pos, const char *data, uint64_t len);
        virtual char at(uint64_t pos);
        virtual uint64_t find(uint64_t begin, uint64_t end, const char* buf, uint64_t len);
        virtual std::pair<uint64_t, uint64_t> trim(uint64_t begin, uint64_t end) const;
        virtual std::pair<char*, uint64_t> chunk(uint64_t pos) const;
        virtual std::pair<uint64_t, uint64_t> segment(uint64_t pos) const;
        virtual uint64_t size() const;
        virtual void resize(uint64_t size);
        virtual const std::string& filename() const;
        virtual DataBufferImpl* getCopy() const;
    private:
        FileBuffer();
    private:
        std::unique_ptr<MMapFile> file_;
        std::shared_ptr<FileHolder> holder_;
        mutable std::mutex mutex_;
        uint64_t window_;
    };

} // namespace fastcgi

#endif // _FASTCGI_REQUEST_CACHE_FILE_BUFFER_H_
