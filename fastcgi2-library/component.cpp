#include "settings.h"

#include "fastcgi2/component.h"
#include "fastcgi2-ext/ComponentSet.h"
#include "fastcgi2-ext/loader.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

    ComponentContext::~ComponentContext() {
    }

    Component::Component(ComponentContext *context) : context_(context) {
    }

    Component::~Component() {
    }

    ComponentContext* Component::context() {
        return context_;
    }

    const ComponentContext* Component::context() const {
        return context_;
    }

} // namespace fastcgi
