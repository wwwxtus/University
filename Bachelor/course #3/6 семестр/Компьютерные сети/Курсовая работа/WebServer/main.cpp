#include "server_backup/HttpServer.h"

int main() {
    HttpServer server(8000, false); 
    server.serve();
    return 0;
}