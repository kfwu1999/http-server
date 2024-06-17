/**
 * \file include/http.h
 */

#pragma once

#ifndef HTTP_H_
#define HTTP_H_

#include <string>
#include <unordered_map>

namespace http {


/**
 * \brief A enum class for http status code
 *
 * \ref: https://github.com/j-ulrich/http-status-codes-cpp
 */
enum class HttpStatusCode {
    OK         = 200,
    NotFound   = 404,
    BadRequest = 400,
};


/**
 * \brief Converts an HttpStatusCode to its corresponding string representation.
 *
 * \param code: The HttpStatusCode value to convert.
 * \return A string representing the HTTP status code. (empty string for unknown codes).
 */
std::string statusCode2str(HttpStatusCode code);


/**
 * \brief A class for building http response.
 */
class HttpResponseBuilder {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Default constructor.
     */
    HttpResponseBuilder() = default;

    /**
     * \brief Default destructor.
     */
    ~HttpResponseBuilder() = default;

/**/
public:
    /**
     * \brief Sets the HTTP status code for the response.
     *
     * \param code: HttpStatusCode enum value representing the status code.
     */
    void setStatusCode(HttpStatusCode code);

    /**
     * \brief Sets a header for the http response.
     *
     * \param headerName: The header name.
     * \param headerValue: The header value.
     */
    void setHeader(const std::string& headerName, const std::string& headerValue);

    /**
     * \brief Sets the body of the HTTP response.
     *
     * \param body The string content to set as the response body.
     */
    void setBody(const std::string& bodh);

    /**
     * \brief Builds the http response message.
     *
     * \return A string containing the complete HTTP response message.
     */
    std::string build();


/**/
private:
    HttpStatusCode m_statusCode;
    std::string m_body;
    std::unordered_map<std::string, std::string> m_headers;
};


/**
 * \brief A class for handling http request.
 */
class HttpRequestHandler {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Default constructor.
     */
    HttpRequestHandler() = default;

    /**
     * \brief Default destructor.
     */
    ~HttpRequestHandler() = default;

/**/
public:
    /**
     * \brief Handles an http request and generates a response.
     *
     * Parses the provided raw http request string, processes it, and generates an 
     * appropriate HTTP response, which is returned as a string.
     *
     * \param request: The raw http request string.
     * \return A string containing the generated HTTP response.
     */
    std::string handleRequest(const std::string& request);

/**/
private:
    /**
     * \brief Parses the http request.
     *
     * Extracts the http method, path, version, headers, and body from the provided 
     * raw http request string.
     *
     * \param request: The raw request string.
     */
    void parseRequest(const std::string& request);

    /**
     * \brief Handles the http GET request.
     *
     * Processes the parsed http GET request and prepares an appropriate http 
     * response using the provided HttpResponse object.
     *
     * \param responseBuilder: The HttpResponse object to build the response.
     */
    void handleGetRequest(HttpResponseBuilder& responseBuilder);

/**/
private:
    std::string m_method;
    std::string m_path;
    std::string m_version;
    std::unordered_map<std::string, std::string> m_headers;
    std::string m_body;
};

} // namespace http::

#endif // HTTP_H_
