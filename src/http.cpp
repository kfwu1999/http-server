/**
 * \file src/http.cpp
 */

#include <exception>
#include <sstream>
#include <unordered_map>

#include "http.h"
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
    HTTP_INFO("Built HTTP response of length {}", response.size());
    return response;
}



std::string HttpRequestHandler::handleRequest(const std::string& request) {
    // 
    HTTP_TRACE("Handling HTTP request of length {}", request.size());

    // 
    HttpResponseBuilder responseBuilder;

    // 
    try {
        //
        parseRequest(request);

        // 
        if (m_method == "GET") {
            handleGetRequest(responseBuilder);
        } else {
            responseBuilder.setStatusCode(HttpStatusCode::BadRequest);
            HTTP_WARN("Unsupported HTTP method: {}", m_method);
        }

    } catch (const std::exception& e) {
        HTTP_ERROR("Error handling request: {}", e.what());
        responseBuilder.setStatusCode(HttpStatusCode::BadRequest);
    }

    // 
    std::string response = responseBuilder.build();
    HTTP_INFO("Handled request, generated response of length {}", response.size());
    return response;
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
    HTTP_INFO("Parsed request line: method = {}, path = {}, version = {}", m_method, m_path, m_version);

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

            HTTP_INFO("Parsed header: {}: {}", headerName, headerValue);
        }
    }

    //
    while (std::getline(requestStream, line)) {
        m_body += line;
    }
    HTTP_INFO("Parsed body of length {}", m_body.size());
}


void HttpRequestHandler::handleGetRequest(HttpResponseBuilder& responseBuilder) {
    // 
    HTTP_TRACE("Handling GET request for path '{}'", m_path);

    // 
    if (m_path == "/") {
        responseBuilder.setStatusCode(HttpStatusCode::OK);
        HTTP_INFO("Responding to GET request for '/'");
    }
    else if (m_path == "/echo") {
        handleEcho(responseBuilder);
        HTTP_INFO("Responding to GET request for '/echo'");
    }
    else {
        responseBuilder.setStatusCode(HttpStatusCode::NotFound);
        HTTP_WARN("Resource not found for path {}", m_path);
    }
}


void HttpRequestHandler::handleEcho(HttpResponseBuilder& responseBuilder) {
    // 
    responseBuilder.setStatusCode(HttpStatusCode::OK);
    responseBuilder.setHeader("Content-Type", "text/plain");

    // 
    std::string responseBody = "Echoing request details:\r\n";
    responseBody += "Method: " + m_method + "\r\n";
    responseBody += "Path: " + m_path + "\r\n";
    responseBody += "Version: " + m_version + "\r\n";
    responseBody += "Headers:\r\n";

    for (const auto& header : m_headers) {
        responseBody += "- " + header.first + ": " + header.second + "\r\n";
    }

    // 
    responseBody += "Body:\r\n" + m_body + "\r\n";
    responseBuilder.setBody(responseBody);
}


} // namespace http::
