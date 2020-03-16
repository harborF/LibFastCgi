// Fastcgi Daemon - framework for design highload FastCGI applications on C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef _FASTCGI_DATA_BUFFER_H_
#define _FASTCGI_DATA_BUFFER_H_

#include <stdint.h>
#include <utility>
#include <memory>

namespace fastcgi
{
    /**
     * Chunked storage with shared internal buffer
     */

    class DataBufferImpl;

    class DataBuffer
    {
    public:
        static DataBuffer create(const char *data, uint64_t size);
        static DataBuffer create(DataBufferImpl *impl);

        DataBuffer();
        DataBuffer(DataBuffer buffer, uint64_t begin, uint64_t end);

        char at(uint64_t pos) const;
        bool isNil() const;
        bool empty() const;
        uint64_t size() const;
        void resize(uint64_t size);

        class SegmentIterator;
        friend class SegmentIterator;

        SegmentIterator begin() const;
        SegmentIterator end() const;

        DataBuffer trim() const;
        DataBuffer trimn(uint64_t b, uint64_t e) const;

        bool split(const std::string &delim, DataBuffer &first, DataBuffer &second) const;
        bool split(char delim, DataBuffer &first, DataBuffer &second) const;

        bool startsWith(const std::string &data) const;
        bool startsWithCI(const std::string &data) const;

        bool endsWith(const std::string &data) const;
        bool endsWithCI(const std::string &data) const;

        void toString(std::string &str) const;

        uint64_t read(uint64_t pos, char *data, uint64_t len);
        uint64_t write(uint64_t pos, const char *data, uint64_t len);

        uint64_t beginIndex() const;
        uint64_t endIndex() const;

        DataBufferImpl* impl() const;

    private:
        void checkIndex(uint64_t index) const;
        uint64_t find(uint64_t pos, const char* buf, uint64_t len) const;

    private:
        std::shared_ptr<DataBufferImpl> data_;
        uint64_t begin_, end_;
    };

    class DataBuffer::SegmentIterator {
    public:
        SegmentIterator();
        std::pair<char*, uint64_t> operator*() const;
        std::pair<char*, uint64_t>* operator->() const;
        SegmentIterator& operator++();
        SegmentIterator operator++(int);
        SegmentIterator& operator--();
        SegmentIterator operator--(int);
    private:
        friend class DataBuffer;
        friend bool operator==(const SegmentIterator &lhs, const SegmentIterator &rhs);
        friend bool operator!=(const SegmentIterator &lhs, const SegmentIterator &rhs);
        SegmentIterator(const DataBuffer &buffer); // begin iterator
        DataBufferImpl* impl() const;
    private:
        DataBuffer buffer_;
        uint64_t pos_begin_, pos_end_;
        mutable std::pair<char*, uint64_t> data_;
    };

    bool operator==(const DataBuffer::SegmentIterator &lhs, const DataBuffer::SegmentIterator &rhs);
    bool operator!=(const DataBuffer::SegmentIterator &lhs, const DataBuffer::SegmentIterator &rhs);

} // namespace fastcgi

#endif // _FASTCGI_DATA_BUFFER_H_