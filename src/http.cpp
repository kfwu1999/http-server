/**
 * \file src/http.cpp
 */

#include <exception>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "http.h"


namespace http {


std::string statusCode2str(HttpStatusCode code) {
    switch (code) {
        case HttpStatusCode::OK:
            return "OK";
        case HttpStatusCode::BadRequest:
            return "Bad Request";
        case HttpStatusCode::NotFound:
            return "Not Found";
        default:
            return std::string();
    }
}



std::string HttpRequestHandler::handleRequest(const std::string& request) {
    // 
    HttpResponseBuilder responseBuilder;

    // 
    try {
        //
        parseRequest(request);

        // 
        if (m_method == "GET") {
            handleGetRequest(responseBuilder);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
        responseBuilder.setStatusCode(HttpStatusCode::BadRequest);
    }

    // 
    return responseBuilder.build();
}


void HttpRequestHandler::parseRequest(const std::string& request) {
    // 
    std::istringstream requestStream(request);
    std::string line;

    // Parse the request line
    std::getline(requestStream, line);
    std::istringstream lineStream(line);
    lineStream >> m_method;
    lineStream >> m_path;
    lineStream >> m_version;

    // Parse headers
    while (std::getline(requestStream, line) && line != "\r") {
        // 
        auto colonPos = line.find(':');

        // 
        if (colonPos != std::string::npos) {
            // 
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 2);

            //
            auto crPos = headerValue.find_last_of('\r');
            headerValue = headerValue.substr(0, crPos);
            m_headers[headerName] = headerValue;
        }
    }

    //
    while (std::getline(requestStream, line)) {
        m_body += line;
    }
}


void HttpRequestHandler::handleGetRequest(HttpResponseBuilder& responseBuilder) {
    if (m_path == "/") {
        responseBuilder.setStatusCode(HttpStatusCode::OK);
    }
    else {
        responseBuilder.setStatusCode(HttpStatusCode::NotFound);
    }
}



void HttpResponseBuilder::setStatusCode(HttpStatusCode statusCode) {
    m_statusCode = statusCode;
}


void HttpResponseBuilder::setHeader(const std::string& headerName, const std::string& headerValue) {
    m_headers[headerName] = headerValue;
}


void HttpResponseBuilder::setBody(const std::string& body) {
    m_body = body;
}


std::string HttpResponseBuilder::build() {
    // 
    std::string response;

    // status line
    response += "HTTP/1.1 ";
    response += std::to_string(static_cast<int>(m_statusCode)) + " " + statusCode2str(m_statusCode);
    response += "\r\n";

    // headers
    if (!m_headers.empty()) {
        for (const auto& header : m_headers) {
            response += header.first + ": " + header.second + "\r\n";
        }
        response += "\r\n";
    }

    // body
    if (!m_body.empty()) {
        response += m_body;
    }

    // 
    return response;
}


} // namespace http::
