/**
 */

#include "server.h"

#define PORT_NUM 8080

int main() {
    // 
    http::HttpServer server(PORT_NUM);
    server.start();
    server.stop();

    return 0;
}
