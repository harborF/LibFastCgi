/*
 * threaded.c -- A simple multi-threaded FastCGI application.
 */

#ifndef lint
static const char rcsid[] = "$Id: threaded.c,v 1.9 2001/11/20 03:23:21 robs Exp $";
#endif /* not lint */

#include <mutex>
#include <thread>
#include <chrono>
#include "fastcgi/fcgi_config.h"
#include "fastcgi/fcgi_io.h"
#include "fastcgi/fcgi_core.h"

#define THREAD_COUNT 20

static int g_counts[THREAD_COUNT];

static void thread_task(int nTid)
{
    int rc, i;
    char *server_name;

    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    for (;;)
    {
        static std::mutex s_accept_mutex, s_counts_mutex;

        std::unique_lock<std::mutex> lock(s_accept_mutex);
        rc = FCGX_Accept_r(&request);
        if (rc < 0)
            break;

        server_name = FCGX_GetParam("SERVER_NAME", request.envp);

        FCGX_FPrintF(request.out,
            "Content-type: text/html\r\n"
            "\r\n"
            "<title>FastCGI Hello! (multi-threaded C, fcgiapp library)</title>"
            "<h1>FastCGI Hello! (multi-threaded C, fcgiapp library)</h1>"
            "Thread %d, Process %ld<p>"
            "Request counts for %d threads running on host <i>%s</i><p><code>",
            nTid, std::this_thread::get_id(), THREAD_COUNT, server_name ? server_name : "?");

        do {
            fcgi_streambuf cin_buf(request.in);
            fcgi_streambuf cout_buf(request.out);
            fcgi_streambuf cerr_buf(request.err);

            std::cout.rdbuf(&cout_buf);

            std::cout << "Content-type: text/html\r\n"
                "\r\n"
                "<TITLE>echo-cpp</TITLE>\n"
                "<H1>echo-cpp</H1>\n"
                "<H4>PID: " << std::this_thread::get_id() << "</H4>\n"
                "<H4>Request Number: " << g_counts[nTid] << "</H4>\n";
        } while (0);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        do {
            std::unique_lock<std::mutex> lock(s_counts_mutex);
            ++g_counts[nTid];
            for (i = 0; i < THREAD_COUNT; i++)
                FCGX_FPrintF(request.out, "%5d ", g_counts[i]);
        } while (0);

        FCGX_Finish_r(&request);
    }//end if
}

int main(int argc, char **argv)
{
    _putenv("FCGI_WEB_SERVER_ADDRS=127.0.0.1:6000");

    FCGX_Init();

    std::thread threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; ++i)
        threads[i] = std::thread(thread_task, i);

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

