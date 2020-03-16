#include "settings.h"

#include "fastcgi2-ext/ResponseTimeStatistics.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

    ResponseTimeStatistics::ResponseTimeStatistics()
    {}

    ResponseTimeStatistics::~ResponseTimeStatistics()
    {}

} // namespace fastcgi
