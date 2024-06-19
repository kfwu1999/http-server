/**
 * \file src/server.cpp
 */

#include <stdexcept>      // std::runtime_error

#include "server.h"
#include "log.h"
#include "net.h"
#include "http.h"


namespace http {

HttpServer::HttpServer(int port) 
    : m_isRunning(false), m_serverSocket(port)
{
    Log::init();
    HTTP_TRACE("HttpSever created");
}


HttpServer::~HttpServer() {
    Log::shutdown();
}


void HttpServer::start() {
    // 
    HTTP_TRACE("HttpSever start");

    // 
    m_isRunning = true;

    // Setup server sock
    m_serverSocket.createSocket();
    m_serverSocket.enableAddressReuse();
    m_serverSocket.bindToPort();
    m_serverSocket.startListening();

    // 
    SocketRAII clientSocket(m_serverSocket.acceptConnection());
    handleConnection(clientSocket);
}


void HttpServer::stop() {
    // 
    HTTP_TRACE("HttpSever stop");

    m_isRunning = false;
}


void HttpServer::handleConnection(SocketRAII& clientSocket) {
    // 
    char buffer[1024];

    // 
    int bytesRead = read(clientSocket.get(), buffer, sizeof(buffer));
    if (bytesRead < 0) {
        HTTP_ERROR("Failed to read from client socket #{}. Error: {}", clientSocket.get(), strerror(errno));
        throw std::runtime_error("Failed to read");
    }
    HTTP_INFO("Read {} bytes from client socket #{}", bytesRead, clientSocket.get());

    // process the request and get the response
    HttpRequestHandler handler;
    std::string response = handler.handleRequest(std::string(buffer));

    // send response back to client
    int bytesSent = send(clientSocket.get(), response.c_str(), response.size(), 0);
    if (bytesSent < 0) {
        HTTP_ERROR("Failed to send response to client socket #{}. Error: {}", clientSocket.get(), strerror(errno));
        throw std::runtime_error("Failed to send response to client socket");
    }
    HTTP_INFO("Sent {} bytes to client socket #{}", bytesSent, clientSocket.get());
}


} // namespace http::
