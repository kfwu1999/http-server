/**
 * \file src/net.cpp
 */

#include <stdexcept>      // std::runtime_error
#include <sys/socket.h>   // socket
#include <netinet/in.h>   // sockaddr_in

#include "net.h"
#include "log.h"

namespace http {


SocketRAII::SocketRAII(int sockfd)
    : m_sockfd(sockfd)
{
}


SocketRAII::SocketRAII(int domain, int type, int protocol) {
    create(domain, type, protocol);
}


SocketRAII::~SocketRAII() {
    if (m_sockfd >= 0)
        close(m_sockfd);
}


SocketRAII::SocketRAII(SocketRAII&& other) noexcept {
    m_sockfd       = other.m_sockfd;
    other.m_sockfd = -1;
}


SocketRAII& SocketRAII::operator=(SocketRAII&& other) noexcept {
    if (this != &other) {
        m_sockfd       = other.m_sockfd;
        other.m_sockfd = -1;
    }
    return *this;
}


void SocketRAII::create(int domain, int type, int protocol) {
    reset(socket(domain, type, protocol));
    if (m_sockfd < 0)
        throw std::runtime_error("Failed to create socket");
}


int SocketRAII::get() const {
    return m_sockfd;
}


void SocketRAII::reset(int newSockFD) {
    if (m_sockfd >= 0)
        close(m_sockfd);
    m_sockfd = newSockFD;
}


ServerSocket::ServerSocket(int port)
    : m_port(port)
{
}


void ServerSocket::createSocket() {
    m_sock.create(AF_INET, SOCK_STREAM, 0);
    HTTP_TRACE("ServerSocket created");
}


void ServerSocket::enableAddressReuse() {
    int opt = 1;
    if (setsockopt(m_sock.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        HTTP_ERROR("Failed to set SO_REUSEADDR option to ServerSocket");
        throw std::runtime_error("Failed to set socket options");
    }
    HTTP_TRACE("SO_REUSEADDR option set on ServerSocket");
}


void ServerSocket::bindToPort() {
    // 
    struct sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(m_port);

    // 
    if (bind(m_sock.get(), reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        HTTP_ERROR("Failed to bind ServerSocket to port {}", m_port);
        throw std::runtime_error("Failed to bind to port");
    }
    HTTP_TRACE("ServerSocket bound to port {}", m_port);
}


void ServerSocket::startListening(int backlog) {
    if (listen(m_sock.get(), backlog)) {
        HTTP_ERROR("Failed to start listening on server socket");
        throw std::runtime_error("Failed to listen");
    }
    HTTP_TRACE("Server socket is listening with backlog {}", backlog);
}


int ServerSocket::acceptConnection() {
    sockaddr_in clientAddr;
    int addrLen = sizeof(sockaddr_in);
    int clientSocket = accept(m_sock.get(), reinterpret_cast<struct sockaddr*>(&clientAddr), reinterpret_cast<socklen_t*>(&addrLen));
    if (clientSocket < 0) {
        HTTP_ERROR("Failed to accept client connection");
        throw std::runtime_error("Failed to accept client connection");
    }
    HTTP_TRACE("Accepted client connection");
    return clientSocket;
}


} // namespece http::
