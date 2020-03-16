#ifndef _FASTCGI_FASTCGI_REQUEST_H_
#define _FASTCGI_FASTCGI_REQUEST_H_

#include <sys/time.h>

#include <fcgiapp.h>
#include <fcgio.h>

#include <memory>
#include <vector>

#include "fastcgi2/RequestStream.h"
#include "fastcgi2-ext/HandlerSet.h"

namespace fastcgi
{
    class Endpoint;
    class Logger;
    class Request;
    class ResponseTimeStatistics;

    class FastcgiRequest : public RequestIOStream {
    public:
        FastcgiRequest(std::shared_ptr<Request> request, Endpoint *endpoint,
            Logger *logger, ResponseTimeStatistics *statistics, const bool logTimes);
        virtual ~FastcgiRequest();
        void attach();
        int accept();

        int read(char *buf, int size);
        int write(const char *buf, int size);
        void write(std::streambuf *buf);

        void setHandlerDesc(const HandlerSet::HandlerDescription *handler);
    private:
        std::shared_ptr<Request> request_;
        Logger *logger_;
        std::string url_;
        Endpoint *endpoint_;
        FCGX_Request fcgiRequest_;
        ResponseTimeStatistics *statistics_;
        const bool logTimes_;
        timeval accept_time_, finish_time_;
        const HandlerSet::HandlerDescription* handler_;
    };

} // namespace fastcgi

#endif // _FASTCGI_FASTCGI_REQUEST_H_
