#include "settings.h"

#include <ctime>
#include <cstdarg>
#include <stdexcept>
#include <strings.h>
#include <iostream>
#include <sstream>

#include "fastcgi2/FastcgiUtils.h"
#include "fastcgi2/config.h"
#include "fastcgi2/logger.h"
#include "fastcgi2/request.h"
#include "fastcgi2/RequestStream.h"

#include <syslog.h>

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace fastcgi
{

    Logger::Logger()
    {
        level_ = DEBUG;
    }

    Logger::~Logger() {
    }

#if 1
    void Logger::writeSuccessLog(const Level level, const char* strInput, const char* strOutput, std::string strDes) {}
    void Logger::writeErrorLog(const Level level, int nErrCode, const char* strInput, std::string strDes) {}
#endif

    void Logger::exiting(const char *function) {
        debug("exiting %s\n", function);
    }

    void Logger::entering(const char *function) {
        debug("entering %s\n", function);
    }

    void Logger::info(const char *format, ...) {
        va_list args;
        va_start(args, format);
        log(INFO, format, args);
        va_end(args);
    }

    void Logger::log(const Level level, const char *format, ...) {
        va_list args;
        va_start(args, format);
        log(level, format, args);
        va_end(args);
    }
    
    void Logger::print(const Level level, const char* APIname, const char *format, ...) {
        std::string new_format("[");
        new_format += APIname;
        new_format += "] ";
        new_format += format;

        va_list args;
        va_start(args, new_format.c_str());
        log(level, new_format.c_str(), args);
        va_end(args);
    }

    void Logger::debug(const char *format, ...) {
        va_list args;
        va_start(args, format);
        log(DEBUG, format, args);
        va_end(args);
    }

    void Logger::error(const char *format, ...) {
        va_list args;
        va_start(args, format);
        log(ERROR, format, args);
        va_end(args);
    }

    void Logger::emerg(const char *format, ...) {
        va_list args;
        va_start(args, format);
        log(EMERGENCY, format, args);
        va_end(args);
    }

    Logger::Level Logger::getLevel() const {
        return level_;
    }

    void Logger::setLevel(const Logger::Level level) {
        setLevelInternal(level);
        level_ = level;
    }

    Logger::Level Logger::stringToLevel(const std::string &name) {
        if (strncasecmp(name.c_str(), "INFO", sizeof("INFO")) == 0) {
            return INFO;
        }
        else if (strncasecmp(name.c_str(), "DEBUG", sizeof("DEBUG")) == 0) {
            return DEBUG;
        }
        else if (strncasecmp(name.c_str(), "ERROR", sizeof("ERROR")) == 0) {
            return ERROR;
        }
        else if (strncasecmp(name.c_str(), "EMERG", sizeof("EMERG")) == 0) {
            return EMERGENCY;
        }
        else {
            std::stringstream stream;
            stream << "bad string to log level cast: " << name;
            throw std::runtime_error(stream.str());
        }
    }

    std::string Logger::levelToString(const Level level) {
        switch (level) {
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case ERROR:
            return "ERROR";
        case EMERGENCY:
            return "EMERG";
        default:
            throw std::runtime_error("bad log level to string cast");
        }
    }

    void Logger::setLevelInternal(const Level level) {
    }

    void Logger::rollOver() {
    }

    BulkLogger::BulkLogger() {
    }

    void BulkLogger::log(Level level, const char *format, va_list args) {
    }

} // namespace fastcgi
