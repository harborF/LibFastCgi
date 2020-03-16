#include "settings.h"

#include <stdexcept>
#include <syslog.h>

#include "fastcgi2/logger.h"
#include "fastcgi2/config.h"
#include "fastcgi2/request.h"
#include "fastcgi2/RequestStream.h"
#include "fastcgi2/ComponentFactory.h"

#include "syslog-logger.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{
    SyslogLogger::SyslogLogger(ComponentContext *context) : Component(context) {
        const Config *config = context->getConfig();
        const std::string componentXPath = context->getComponentXPath();

        ident_ = config->asString(componentXPath + "/ident");

        setLevel(stringToLevel(config->asString(componentXPath + "/level")));
    }

    SyslogLogger::~SyslogLogger() {
    }

    void SyslogLogger::onLoad() {
    }

    void SyslogLogger::onUnload() {
    }

    void SyslogLogger::handleRequest(Request *request, HandlerContext *handlerContext) {
        request->setContentType("text/plain");
        const std::string &action = request->getArg("action");
        if ("setlevel" == action) {
            const std::string &l = request->getArg("level");
            setLevel(stringToLevel(l));
            RequestStream(request) << "level " << l << "successfully set" << std::endl;
        }
        else if ("rollover" == action) {
            rollOver();
            RequestStream(request) << "rollover successful" << std::endl;
        }
        else {
            RequestStream(request) << "bad action" << std::endl;
        }
    }

    void SyslogLogger::log(const Level level, const char *format, va_list args) {
        if (level >= getLevel()) {
            std::string signedFormat = ident_ + ": " + format;
            vsyslog(toSyslogPriority(level), signedFormat.c_str(), args);
        }
    }


    void SyslogLogger::writeSuccessLog(const fastcgi::Logger::Level level, const char* strInput, const char* strOutput, std::string strDes) {
        syslog(toSyslogPriority(level), "%s %s %s", strInput, strOutput, strDes.c_str());
    }

    void SyslogLogger::writeErrorLog(const fastcgi::Logger::Level level, int nErrCode, const char* strInput, std::string strDes) {
        syslog(toSyslogPriority(level), "%d %s %s", nErrCode, strInput, strDes.c_str());

    }

    void SyslogLogger::setLevelInternal(const Level level) {
        priority_ = toSyslogPriority(level);
    }

    void SyslogLogger::rollOver() {
    }

    int SyslogLogger::toSyslogPriority(const Level level) {
        switch (level) {
        case INFO:
            return LOG_INFO;
        case DEBUG:
            return LOG_DEBUG;
        case ERROR:
            return LOG_ERR;
        case EMERGENCY:
            return LOG_EMERG;
        default:
            throw std::logic_error("SyslogLogger: unknown log level");
        }
    }

} //namespace fastcgi

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
    FCGIDAEMON_ADD_DEFAULT_FACTORY("logger", fastcgi::SyslogLogger)
FCGIDAEMON_REGISTER_FACTORIES_END()
