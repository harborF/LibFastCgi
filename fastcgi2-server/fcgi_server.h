#ifndef _FASTCGI_FASTCGI_SERVER_H_
#define _FASTCGI_FASTCGI_SERVER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>

#include "fastcgi2-ext/server.h"

namespace fastcgi
{
    class Config;
    class Request;
    class Logger;
    class Loader;
    class Endpoint;
    class ComponentSet;
    class HandlerSet;
    class RequestsThreadPool;

    class ServerStopper {
    public:
        ServerStopper() : stopped_(false) {}

        void stopped(bool flag) {
            std::unique_lock<std::mutex> lock(mutex_);
            stopped_ = flag;
        }

        bool stopped() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return stopped_;
        }
    private:
        mutable std::mutex mutex_;
        bool stopped_;
    };

    class ActiveThreadCounter {
    public:
        ActiveThreadCounter() : count_(0)
        {}
        void increment() {
            std::unique_lock<std::mutex> lock(mutex_);
            ++count_;
        }
        void decrement() {
            std::unique_lock<std::mutex> lock(mutex_);
            --count_;
        }

        int count() const {
            std::mutex mutex_;
            return count_;
        }

    private:
        std::mutex mutex_;
        int count_;
    };

    class FCGIServer : public Server {
    protected:
        enum Status { NOT_INITED, LOADING, RUNNING };

    public:
        FCGIServer(std::shared_ptr<Globals> globals);
        virtual ~FCGIServer();
        void start();
        void stop();
        void join();

    private:
        virtual const Globals* globals() const;
        virtual Logger* logger() const;
        virtual void handleRequest(RequestTask task);
        void handle(Endpoint *endpoint);
        void monitor();

        std::string getServerInfo() const;

        void pid(const std::string &file);

        void initMonitorThread();
        void initRequestCache();
        void initTimeStatistics();
        void initFastCGISubsystem();
        void initPools();
        void createWorkThreads();

        void stopInternal();

        void stopThreadFunction();

        Status status() const;

    private:
        std::shared_ptr<Globals> globals_;
        std::shared_ptr<ServerStopper> stopper_;

        typedef char ThreadHolder;
        std::shared_ptr<ThreadHolder> active_thread_holder_;

        std::vector<std::shared_ptr<Endpoint> > endpoints_;
        int monitorSocket_;

        RequestCache *request_cache_;
        ResponseTimeStatistics *time_statistics_;

        mutable std::mutex statusInfoMutex_;
        Status status_;

        std::unique_ptr<std::thread> monitorThread_;
        std::unique_ptr<std::thread> stopThread_;
        int stopPipes_[2];

        bool logTimes_;
        std::vector<std::thread> globalPool_;

    };

} // namespace fastcgi

#endif // _FASTCGI_FASTCGI_SERVER_H_
