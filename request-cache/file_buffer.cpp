#include "settings.h"

#include <stdexcept>

#include "fastcgi2-ext/DataBufferImpl.h"
#include "fastcgi2-ext/range.h"

#include "fastcgi2/FastcgiUtils.h"

#include "file_buffer.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    FileBuffer::FileBuffer() : window_(0)
    {}

    FileBuffer::FileBuffer(const char *name, uint64_t window) :
        file_(new MMapFile(name, window)), holder_(new FileHolder(name)),
        window_(file_->window())
    {}

    FileBuffer::~FileBuffer() {
    }

    char FileBuffer::at(uint64_t pos) {
        std::unique_lock<std::mutex> lock(mutex_);
        return file_->at(pos);
    }

    uint64_t FileBuffer::read(uint64_t pos, char *data, uint64_t len) {
        if (pos + len > size()) {
            throw std::runtime_error("Data is out of range");
        }
        std::unique_lock<std::mutex> lock(mutex_);
        uint64_t read_len = len;
        while (read_len > 0) {
            std::pair<char*, uint64_t> cur_chunk = chunk(pos);
            if (NULL == cur_chunk.first || 0 == cur_chunk.second) {
                throw std::runtime_error("Cannot fetch chunk");
            }
            uint64_t cur_len = std::min(cur_chunk.second, read_len);
            memcpy(data, cur_chunk.first, cur_len);
            pos += cur_len;
            data += cur_len;
            read_len -= cur_len;
        }
        return len;
    }

    uint64_t FileBuffer::write(uint64_t pos, const char *data, uint64_t len) {
        if (pos + len > size()) {
            throw std::runtime_error("Data is out of range");
        }
        std::unique_lock<std::mutex> lock(mutex_);
        uint64_t write_len = len;
        while (write_len > 0) {
            std::pair<char*, uint64_t> cur_chunk = chunk(pos);
            if (NULL == cur_chunk.first || 0 == cur_chunk.second) {
                throw std::runtime_error("Cannot fetch chunk");
            }
            uint64_t cur_len = std::min(cur_chunk.second, write_len);
            memcpy(cur_chunk.first, data, cur_len);
            pos += cur_len;
            data += cur_len;
            write_len -= cur_len;
        }
        return len;
    }

    uint64_t
        FileBuffer::find(uint64_t begin, uint64_t end, const char* buf, uint64_t len) {
        if (len > end - begin) {
            return end;
        }
        uint64_t segment = begin / window_;
        uint64_t segment_pos = window_ * segment;
        uint64_t offset = begin - segment_pos;
        bool finish = false;
        std::unique_lock<std::mutex> lock(mutex_);
        while (1) {
            uint64_t pos = segment_pos + offset;
            uint64_t length = window_ - offset + len;
            if (pos + length >= end) {
                length = end - pos;
                finish = true;
            }

            char* range = file_->atRange(pos, length);
            Range base(range, range + length);
            Range substr(buf, buf + len);

            const char* res = base.find(substr);
            if (res != base.end()) {
                return (res - base.begin()) + pos;
            }

            if (finish) {
                break;
            }

            offset = 0;
            segment_pos += window_;
        }
        return end;
    }

    std::pair<uint64_t, uint64_t>
        FileBuffer::trim(uint64_t begin, uint64_t end) const {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while (begin != end && isspace(file_->at(begin))) {
                    ++begin;
                }
            }
            while (begin != end && isspace(end - 1)) {
                --end;
            }
            return std::make_pair(begin, end);
    }

    std::pair<char*, uint64_t> FileBuffer::chunk(uint64_t pos) const {
        return file_->atSegment(pos);
    }

    std::pair<uint64_t, uint64_t> FileBuffer::segment(uint64_t pos) const {
        uint64_t beg = window_ * (pos / window_);
        return std::pair<uint64_t, uint64_t>(pos, beg + window_);
    }

    uint64_t FileBuffer::size() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return file_->size();
    }

    void FileBuffer::resize(uint64_t size) {
        std::unique_lock<std::mutex> lock(mutex_);
        file_->resize(size);
    }

    const std::string& FileBuffer::filename() const {
        return holder_.get() ? holder_->filename : StringUtils::EMPTY_STRING;
    }

    DataBufferImpl* FileBuffer::getCopy() const {
        std::unique_ptr<FileBuffer> buffer(new FileBuffer);
        buffer->file_.reset(file_->clone());
        buffer->holder_ = holder_;
        buffer->window_ = window_;
        return buffer.release();
    }

} // namespace fastcgi
