#include "HttpServer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

HttpServer::HttpServer(int port, bool useSSL) : port(port), useSSL(useSSL), pool(4) {
    setupSocket();
    if (useSSL) {
        ssl_ctx = initSSL("cert.pem", "key.pem");

    }
}

void HttpServer::setupSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
}

void HttpServer::serve() {
    acceptConnections();
}

void HttpServer::acceptConnections() {
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        pool.enqueue([client_fd, this]() {
            if (useSSL) {
                SSL* ssl = SSL_new(ssl_ctx);
                SSL_set_fd(ssl, client_fd);
                SSL_accept(ssl);
                handleRequest(ssl);
                SSL_shutdown(ssl);
                SSL_free(ssl);
            } else {
                handleRequest(client_fd);
            }
            close(client_fd);
        });
    }
}
