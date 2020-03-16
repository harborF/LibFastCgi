#include "settings.h"

#include <stdexcept>

#include "fastcgi2-ext/range.h"
#include "fastcgi2-ext/StringBuffer.h"

#include "fastcgi2/FastcgiUtils.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    StringBuffer::StringBuffer(const char *data, uint64_t size) :
        data_(new std::vector<char>(data, data + size))
    {}

    StringBuffer::~StringBuffer()
    {}

    uint64_t StringBuffer::read(uint64_t pos, char *data, uint64_t len) {
        memcpy(data, &((*data_)[pos]), len);
        return len;
    }

    uint64_t StringBuffer::write(uint64_t pos, const char *data, uint64_t len) {
        memcpy(&((*data_)[pos]), data, len);
        return len;
    }

    char StringBuffer::at(uint64_t pos) {
        return data_->at(pos);
    }

    uint64_t StringBuffer::find(uint64_t begin, uint64_t end, const char* buf, uint64_t len) {
        if (len > end - begin) {
            return end;
        }
        char* first = &((*data_)[0]);
        Range base(first + begin, first + end);
        Range substr(buf, buf + len);
        return base.find(substr) - first;
    }

    std::pair<uint64_t, uint64_t>
        StringBuffer::trim(uint64_t begin, uint64_t end) const {
        char* first = &((*data_)[0]);
        Range base(first + begin, first + end);
        Range trimmed = base.trim();
        return std::pair<uint64_t, uint64_t>(trimmed.begin() - first, trimmed.end() - first);
    }

    std::pair<char*, uint64_t>
        StringBuffer::chunk(uint64_t pos) const {
        return std::pair<char*, uint64_t>(&((*data_)[0]) + pos, data_->size() - pos);
    }

    std::pair<uint64_t, uint64_t>
        StringBuffer::segment(uint64_t pos) const {
        return std::pair<uint64_t, uint64_t>(pos, size());
    }

    uint64_t StringBuffer::size() const {
        return data_->size();
    }

    void StringBuffer::resize(uint64_t size) {
        data_->resize(size);
    }

    const std::string& StringBuffer::filename() const {
        return StringUtils::EMPTY_STRING;
    }

    DataBufferImpl* StringBuffer::getCopy() const {
        return new StringBuffer(*this);
    }

} // namespace fastcgi
