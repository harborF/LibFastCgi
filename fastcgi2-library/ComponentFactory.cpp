#include "settings.h"
#include "fastcgi2/ComponentFactory.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

    ComponentFactory::ComponentFactory() {
    }

    ComponentFactory::~ComponentFactory() {
    }

} // namespace fastcgi
