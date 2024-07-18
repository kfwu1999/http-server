/**
 * \file include/request.h
 */

#pragma once

#ifndef REQUEST_H_
#define REQUEST_H_

#include <string>
#include <unordered_map>
#include <mutex>

#include "response.h"
#include "cache.h"


namespace http {


/**
 */
struct HttpRequest {
    /**
     * \brief Parses the raw HTTP request string.
     *
     * \param request: The raw request string.
     */
    void parse(const std::string& request);

    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};


/**
 */
class HttpRequestHandler {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Default constructor
     */
    HttpRequestHandler(LRUCache& cache, std::mutex& cacheMtx)
        : r_cache(cache), r_cacheMtx(cacheMtx) {}

    /**
     * \brief Default destructor
     */
    ~HttpRequestHandler() = default;

/**/
public:
    /**
     * \brief Handles an HTTP request and generates a response.
     *
     * Parses the provided raw HTTP request string, process it, and generate 
     * HTTP response.
     *
     * \param request: The raw HTTP request string.
     * \return A string containing the generated HTTP response.
     */
    std::string handleRequest(const std::string& request);

/**/
private:
    /**
     * \brief Handles the HTTP GET request.
     *
     * Processes the parsed HTTP GET request and prepares an appropriate http 
     * response using the provided HttpResponse object.
     *
     * \param httpRequest: The HTTP request containing the uploaded file data.
     * \param responseBuilder: The HttpResponse object to build the response.
     */
    void handleGetRequest(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder);

    /**
     * \brief Handles the HTTP POST request.
     *
     * Processes the parsed HTTP POST request and prepares an appropriate http 
     * response using the provided HttpResponse object.
     *
     * \param httpRequest: The HTTP request containing the uploaded file data.
     * \param responseBuilder: The HttpResponse object to build the response.
     */
    void handlePostRequest(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder);

/**/
private:
    /**
     * \brief Handles the HTTP GET request for the "/echo" path.
     *
     * Respond the echoes of request details.
     *
     * \param httpRequest: The HTTP request containing the uploaded file data.
     * \param responseBuilder The HttpResponse object to build the response.
     */
    void handleEcho(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder);

    /**
     * \brief Handles HTTP POST requests for uploading files.
     *
     * Stores the upload content to the txt file in predefined directory.
     *
     * \param httpRequest: The HTTP request containing the uploaded file data.
     * \param responseBuilder The HttpResponse object to build the response.
     */
    void handleUpload(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder);


/**/
private:
    /**
     * \brief Serves a static file based on the request path.
     *
     * \param httpRequest: The HTTP request containing the uploaded file data.
     * \param responseBuilder: The HttpResponse object to build the response.
     */
    void serveStaticFile(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder);

/**/
private:
    /**
     * \brief Serves an status code image based on the status code.
     *
     * \param responseBuilder: The HttpResponse object to build the response.
     * \param statusCode: The HTTP status code.
     *
     * \note This function does not call serveStaticFile to avoid recursive
     * calls, which can occur when an error happens while serving a static file.
     */
    void serveStatusCodeImage(HttpResponseBuilder& responseBuilder, const HttpStatusCode& statusCode);

private:
    LRUCache&   r_cache;
    std::mutex& r_cacheMtx;
};


} // namespace http::

#endif // REQUEST_H_
