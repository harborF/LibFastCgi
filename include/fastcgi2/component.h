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

#ifndef _FASTCGI_COMPONENT_H_
#define _FASTCGI_COMPONENT_H_

#include "fastcgi2/FastcgiUtils.h"

namespace fastcgi
{
    class Config;
    class Component;

    class ComponentContext : private noncopyable
    {
    public:
        virtual ~ComponentContext();

        virtual const Config* getConfig() const = 0;
        virtual std::string getComponentXPath() const = 0;

        template<typename T>
        T* findComponent(const std::string &name) {
            return dynamic_cast<T*>(findComponentInternal(name));
        }

    protected:
        virtual Component* findComponentInternal(const std::string &name) const = 0;
    };

    class Component : private noncopyable
    {
    public:
        Component(ComponentContext *context);
        virtual ~Component();

        virtual void onLoad() = 0;
        virtual void onUnload() = 0;

    protected:
        ComponentContext* context();
        const ComponentContext* context() const;

    private:
        ComponentContext *context_;
    };

} // namespace fastcgi

#endif // _FASTCGI_COMPONENT_H_
