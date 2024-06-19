/**
 * \file include/net.h
 */

#pragma once

#ifndef NET_H_
#define NET_H_

#define DEFAULT_BACKLOG 10


#include <sys/socket.h>   // socket
#include <unistd.h>       // close


namespace http {

/**
 * \brief A wrapper class for socket using RAII.
 */
class SocketRAII {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Construct a SocketRAII by a existing socket fd.
     *
     * \param sockfd: The file descriptor of existing socket.
     */
    explicit SocketRAII(int sockfd = -1);

    /**
     * \brief Constructs a SocketRAII object by creating a new socket.
     *
     * \param domain: The communicatoin domain. (e.g. AF_INET for IPv4)
     * \param type: The communication semantics. (e.g. SOCK_STREAM for TCP)
     * \param protocol: The protocol to be used with the socket. (e.g. 0)
     */
    SocketRAII(int domain, int type, int protocol);

    /**
     * \brief Destructor
     *
     * Closes the socket fd if it is valid.
     */
    ~SocketRAII();

    /**
     * \brief Delete the copy constructor, make SocketRAII move-only
     */
    SocketRAII(const SocketRAII& other) = delete;

    /**
     * \brief Delete the copy assignment operator, make SocketRAII move-only
     */
    SocketRAII& operator=(const SocketRAII& other) = delete;

    /**
     * \brief Move constructor
     */
    SocketRAII(SocketRAII&& other) noexcept;

    /**
     * \brief Move constructor
     */
    SocketRAII& operator=(SocketRAII&& other) noexcept;

/**/
public:
    /**
     */
    void create(int domain, int type, int protocol);

    /**
     * \brief Get the socket fd.
     *
     * \return sockfd
     */
    int get() const;

    /**
     * \brief Resets the socket file descriptor.
     * 
     * \param newSockFD The new socket file descriptor to manage, or -1 to indicate no socket.
     */
    void reset(int newSockFD = -1);

private:
    int m_sockfd;
};



/**
 * \breif A class for server-side socket
 */
class ServerSocket {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Construct a ServerSocket.
     *
     * Construct a IPv4 TCP SocketRAII for server.
     *
     * \param port: The port number to bind server socket.
     */
    explicit ServerSocket(int port);

    /**
     * \brief Destructor
     */
    ~ServerSocket() = default;

/**/
public:
    /**
     */
    void createSocket();

    /**
     * \brief Set the SO_REUSEADDR socket option on a server socket. 
     *
     * Enabling quick restarts using the same port number immediately after 
     * stopping the server.
     */
    void enableAddressReuse();

    /**
     * \brief Binds the server socket to the specified port.
     *
     * \throw std::runtime_error if the bind operation fails.
     */
    void bindToPort();

    /**
     * \brief Make the server socket in listening state.
     *
     * \param backlog: The maximum length of the queue for pending connections. (default = 10)
     * \throw std::runtime_error if the listen operation fails.
     */
    void startListening(int backlog = DEFAULT_BACKLOG);

    /**
     * \brief Accepts an incoming client connection.
     *
     * \return client fd
     * \throw std::runtime_error if the accept if the accept failed.
     */
    int acceptConnection();

private:
    int        m_port;
    SocketRAII m_sock;
};


} // namespace http::

#endif // NET_H_
