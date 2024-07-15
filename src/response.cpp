/**
 * \file src/response.cpp
 */

#include "response.h"
#include "log.h"


namespace http {

std::string statusCode2str(HttpStatusCode code) {
    switch (code) {
        case HttpStatusCode::OK:
            return "OK";
        case HttpStatusCode::BadRequest:
            return "Bad Request";
        case HttpStatusCode::NotFound:
            return "Not Found";
        case HttpStatusCode::InternalServerError:
            return "Internal Server Error";
        default:
            return std::string();
    }
}


void HttpResponseBuilder::setStatusCode(HttpStatusCode statusCode) {
    m_statusCode = statusCode;
}


void HttpResponseBuilder::setHeader(const std::string& headerName, const std::string& headerValue) {
    m_headers[headerName] = headerValue;
}


void HttpResponseBuilder::setBody(const std::string& body) {
    m_body.assign(body.begin(), body.end());
}


void HttpResponseBuilder::setBody(const std::vector<unsigned char>& body) {
    m_body = body;
}


std::string HttpResponseBuilder::build() {
    // 
    HTTP_TRACE("Building HTTP response with status code {}", static_cast<int>(m_statusCode));

    // 
    std::string response;

    // status line
    response += "HTTP/1.1 ";
    response += std::to_string(static_cast<int>(m_statusCode)) + " " + statusCode2str(m_statusCode);
    response += "\r\n";

    // headers
    for (const auto& header : m_headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    response += "\r\n";

    // body
    response.insert(response.end(), m_body.begin(), m_body.end());

    // 
    HTTP_INFO("Built HTTP response of length {}", response.size());
    return response;
}


} // namespace http::
