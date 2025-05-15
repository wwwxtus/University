#ifndef SSLCONTEXT_H
#define SSLCONTEXT_H

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* initSSL(const char* cert, const char* key);

#endif