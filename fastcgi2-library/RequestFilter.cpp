#include "settings.h"

#include "fastcgi2-ext/RequestFilter.h"
#include "fastcgi2/request.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    RegexFilter::RegexFilter(const std::string &regex) : regex_(regex)
    {}

    RegexFilter::~RegexFilter()
    {}

    bool RegexFilter::check(const std::string &value) const {
        return std::regex_match(value, regex_);
    }

    UrlFilter::UrlFilter(const std::string &regex) : regex_(regex)
    {}

    UrlFilter::~UrlFilter()
    {}

    bool UrlFilter::check(const Request *request) const {
        return regex_.check(request->getScriptName());
    }

    HostFilter::HostFilter(const std::string &regex) : regex_(regex)
    {}

    HostFilter::~HostFilter()
    {}

    bool HostFilter::check(const Request *request) const {
        return regex_.check(request->getHost());
    }

    PortFilter::PortFilter(const std::string &regex) : regex_(regex)
    {}

    PortFilter::~PortFilter()
    {}

    bool PortFilter::check(const Request *request) const {
        std::string port = i_cast_str<unsigned short>(request->getServerPort());
        return regex_.check(port);
    }

    AddressFilter::AddressFilter(const std::string &regex) : regex_(regex)
    {}

    AddressFilter::~AddressFilter()
    {}

    bool AddressFilter::check(const Request *request) const {
        return regex_.check(request->getServerAddr());
    }

    ParamFilter::ParamFilter(const std::string &name, const std::string &regex) :
        name_(name), regex_(regex)
    {}

    ParamFilter::~ParamFilter()
    {}

    bool ParamFilter::check(const Request *request) const {
        if (!request->hasArg(name_)) {
            return false;
        }
        return regex_.check(request->getArg(name_));
    }


} // namespace fastcgi
