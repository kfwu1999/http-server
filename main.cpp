/**
 */

#include "server.h"

#define PORT_NUM 8080

int main() {
    // 
    std::size_t cacheSize = 10;
    http::HttpServer server(PORT_NUM, cacheSize);
    server.start();
    server.stop();

    return 0;
}
