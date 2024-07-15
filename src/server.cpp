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
    while (m_isRunning) {
        int clientfd = m_serverSocket.acceptConnection();
        m_threadPool.submit(std::bind(handleConnection, clientfd));
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
    HttpRequestHandler handler;
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
