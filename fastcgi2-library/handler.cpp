#include "settings.h"

#include "fastcgi2-ext/HandlerContext.h"
#include "fastcgi2-ext/HandlerSet.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    HandlerContext::~HandlerContext() {
    }

    std::string HandlerContextImpl::getParam(const std::string &name) const {
        ParamsMapType::const_iterator itr = params_.find(name);
        if (itr != params_.end()) {
            return itr->second;
        }
        return std::string();
    }

    void HandlerContextImpl::setParam(const std::string &name, const std::string &value) {
        params_[name] = value;
    }

    Handler::Handler() {
    }

    Handler::~Handler() {
    }

    void Handler::onThreadStart() {
    }

} // namespace fastcgi
