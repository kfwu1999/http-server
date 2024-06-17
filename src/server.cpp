/**
 * \file src/server.cpp
 */

#include <stdexcept>      // std::runtime_error

#include "server.h"
#include "net.h"
#include "http.h"


namespace http {

HttpServer::HttpServer(int port) 
    : m_isRunning(false), m_serverSocket(port)
{
}


void HttpServer::start() {
    // 
    m_isRunning = true;

    // Setup server sock
    m_serverSocket.enableAddressReuse();
    m_serverSocket.bindToPort();
    m_serverSocket.startListening();

    // 
    SocketRAII clientSocket(m_serverSocket.acceptConnection());
    handleConnection(clientSocket);
}


void HttpServer::stop() {
    m_isRunning = false;
}


void HttpServer::handleConnection(SocketRAII& clientSocket) {
    // 
    char buffer[1024];
    long bytesRead = read(clientSocket.get(), buffer, sizeof(buffer));

    if (bytesRead < 0)
        throw std::runtime_error("Failed to read");

    // process the request and get the response
    HttpRequestHandler handler;
    std::string response = handler.handleRequest(std::string(buffer));

    // send response back to client
    send(clientSocket.get(), response.c_str(), response.size(), 0);
}


} // namespace http::
