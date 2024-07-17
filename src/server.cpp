/**
 * \file src/server.cpp
 */

#include <stdexcept>   // std::runtime_error

#include "server.h"
#include "log.h"
#include "net.h"
#include "request.h"
#include "thread_pool.hpp"


#define BUFFER_SIZE 1024

namespace http {

HttpServer::HttpServer(int port, std::size_t cacheSize) 
    : m_isRunning(false), m_serverSocket(port), m_cache(cacheSize)
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
    while (m_isRunning) {
        int clientfd = m_serverSocket.acceptConnection();
        // The `HttpRequestHandler` in ``handleConnection`` will access member variables
        // `m_cache` and `m_cacheMtx`, so the `handleConnection` can't be static.
        // Need to pass `this` into thread function.
        m_threadPool.submit(std::bind(&HttpServer::handleConnection, this, clientfd));
    }
}


void HttpServer::stop() {
    // 
    HTTP_TRACE("HttpSever stop");

    m_isRunning = false;
}


void HttpServer::handleConnection(int clientfd) {
    // Use SocketRAII to manage the lifecycle of the client socket
    SocketRAII clientSocket(clientfd);

    // read raw data of request from client socket
    char buffer[BUFFER_SIZE];
    int bytesRead = read(clientSocket.get(), buffer, sizeof(buffer));
    if (bytesRead < 0) {
        HTTP_ERROR("Failed to read from client socket #{}. Error: {}", clientSocket.get(), strerror(errno));
        throw std::runtime_error("Failed to read");
    }
    HTTP_INFO("Read {} bytes from client socket #{}", bytesRead, clientSocket.get());

    // process the request and get the response
    HttpRequestHandler handler(m_cache, m_cacheMtx);
    std::string response = handler.handleRequest(std::string(buffer));

    // send response back to client
    int bytesSent = send(clientSocket.get(), response.c_str(), response.size(), 0);
    if (bytesSent < 0) {
        HTTP_ERROR("Failed to send response to client socket #{}. Error: {}", clientSocket.get(), strerror(errno));
        throw std::runtime_error("Failed to send response to client socket");
    }
    HTTP_INFO("Response sent to client socket #{}", clientSocket.get());
}


} // namespace http::
