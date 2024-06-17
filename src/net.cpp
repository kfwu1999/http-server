/**
 * \file src/net.cpp
 */

#include <stdexcept>      // std::runtime_error
#include <sys/socket.h>   // socket
#include <netinet/in.h>   // sockaddr_in

#include "net.h"

namespace http {


SocketRAII::SocketRAII(int sockfd)
    : m_sockfd(sockfd)
{
}


SocketRAII::SocketRAII(int domain, int type, int protocol) {
    m_sockfd = socket(domain, type, protocol);
    if (m_sockfd < 0)
        throw std::runtime_error("Failed to create socket");
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


int SocketRAII::get() const {
    return m_sockfd;
}


ServerSocket::ServerSocket(int port)
    : m_port(port), m_sock(AF_INET, SOCK_STREAM, 0) 
{
}


void ServerSocket::enableAddressReuse() {
    int opt = 1;
    if (setsockopt(m_sock.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Failed to set socket options");
}


void ServerSocket::bindToPort() {
    // 
    struct sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(m_port);

    // 
    if (bind(m_sock.get(), reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Failed to bind to port");
    }
}


void ServerSocket::startListening(int backlog) {
    if (listen(m_sock.get(), backlog))
        throw std::runtime_error("Failed to listen");
}


int ServerSocket::acceptConnection() {
    sockaddr_in clientAddr;
    int addrLen = sizeof(sockaddr_in);
    int clientSocket = accept(m_sock.get(), reinterpret_cast<struct sockaddr*>(&clientAddr), reinterpret_cast<socklen_t*>(&addrLen));
    if (clientSocket < 0)
        throw std::runtime_error("Failed to accept connection");
    return clientSocket;
}


} // namespece http::
