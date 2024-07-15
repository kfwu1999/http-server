/**
 * \file src/response.h
 */

#pragma once

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <vector>
#include <unordered_map>


namespace http {

/**
 * \brief A enum class for http status code
 *
 * \ref: https://github.com/j-ulrich/http-status-codes-cpp
 */
enum class HttpStatusCode {
    OK                  = 200,
    NotFound            = 404,
    BadRequest          = 400,
    InternalServerError = 500,
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
    void setBody(const std::string& body);
    void setBody(const std::vector<unsigned char>& body);

    /**
     * \brief Builds the http response message.
     *
     * \return A string containing the complete HTTP response message.
     */
    std::string build();


/**/
private:
    HttpStatusCode m_statusCode;
    std::vector<unsigned char> m_body;
    std::unordered_map<std::string, std::string> m_headers;
};


} // namespace http::


#endif // RESPONSE_H_
