#include "SSLContext.h"
#include <iostream>

SSL_CTX* initSSL(const char* cert, const char* key) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) ||
        !SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM)) {
        std::cerr << "Failed to load certificate or key\n";
        exit(1);
    }
    return ctx;
}