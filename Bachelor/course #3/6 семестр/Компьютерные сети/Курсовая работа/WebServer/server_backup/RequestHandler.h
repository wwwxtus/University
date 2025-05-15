#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <string>
#include <openssl/ssl.h>
#include <sys/socket.h>

void handleRequest(int client_fd);
void handleRequest(SSL* ssl);

#endif