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

#ifndef _FASTCGI_DETAILS_STRING_BUFFER_H_
#define _FASTCGI_DETAILS_STRING_BUFFER_H_

#include <string>
#include <vector>
#include <memory>

#include "fastcgi2/DataBuffer.h"
#include "fastcgi2-ext/DataBufferImpl.h"

namespace fastcgi
{
    class StringBuffer : public DataBufferImpl {
    public:
        StringBuffer(const char *data, uint64_t size);
        virtual ~StringBuffer();
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
        std::shared_ptr<std::vector<char> > data_;
    };

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_STRING_BUFFER_H_
