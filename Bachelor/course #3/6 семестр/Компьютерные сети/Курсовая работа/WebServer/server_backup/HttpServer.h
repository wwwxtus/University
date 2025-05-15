#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include "RequestHandler.h"
#include "ThreadPool.h"
#include "SSLContext.h"

class HttpServer {
public:
    HttpServer(int port, bool useSSL = false);
    void serve();

private:
    int port;
    bool useSSL;
    int server_fd;
    SSL_CTX* ssl_ctx;
    ThreadPool pool;

    void setupSocket();
    void acceptConnections();
};

#endif