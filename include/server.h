/**
 * \file include/server.h
 */

#pragma once

#ifndef SERVER_H_
#define SERVER_H_

#include "net.h"
#include "thread_pool.hpp"


namespace http {


/**
 * \brief 
 */
class HttpServer {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Construct a http sever with specific port
     *
     * Init server socket with specific port
     *
     * \param port: The port number that the server will listen for connections.
     */
    explicit HttpServer(int port);

    /**
     * \brief Destructor
     *
     * Shutdown the logger when HttpServer destruct.
     */
    ~HttpServer();

/**/
public:
    /**
     * \brief Start the http server
     *
     * Setup server socket, bind port to server socket, start listening connections, 
     * and handle incoming request
     */
    void start();

    /**
     * \brief Stop the http server
     */
    void stop();

/**/
private:
    /**
     * \brief Handles the connection from a client.
     *
     * Read request from server socket, and send back the corresponding http response.
     *
     * \param clientfd: The sockfd of client socket.
     */
    static void handleConnection(int clientfd);

/**/
private:
    bool         m_isRunning;
    ServerSocket m_serverSocket;
    ThreadPool   m_threadPool;
};


} // namespace http::

#endif // SERVER_H_
