/**
 * \file src/http.cpp
 */

#include <exception>
#include <sstream>
#include <unordered_map>
#include <filesystem>

#include "http.h"
#include "file.h"
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
            HTTP_ERROR("Unsupported HTTP method: {}", m_method);
            serveStatusCodeImage(responseBuilder, HttpStatusCode::BadRequest);
        }

    } catch (const std::exception& e) {
        HTTP_ERROR("Error handling request: {}", e.what());
        serveStatusCodeImage(responseBuilder, HttpStatusCode::BadRequest);
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
    if (m_path == "/")
        m_path = "/home.html";

    // 
    if (m_path.substr(0, 5) == "/echo") {
        handleEcho(responseBuilder);
        HTTP_INFO("Responding to GET request for '/echo'");
    }
    else {
        serveStaticFile(responseBuilder);
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


void HttpRequestHandler::serveStaticFile(HttpResponseBuilder& responseBuilder) {
    // 
    try {
        // 
        std::string filepath = mapUrlToFilePath(m_path);
        std::string extension = std::filesystem::path(filepath).extension().string();

        // read file content
        std::vector<unsigned char> fileContent = loadFile(filepath);

        // 
        responseBuilder.setStatusCode(HttpStatusCode::OK);
        responseBuilder.setHeader("Content-Type", getMimeType(extension));
        responseBuilder.setBody(fileContent);

        // 
        HTTP_INFO("Served static file '{}'", filepath);
    }
    catch (const std::runtime_error& e) {
        HTTP_ERROR("File not found: {}", e.what());
        serveStatusCodeImage(responseBuilder, HttpStatusCode::NotFound);
    }
    catch (const std::exception& e) {
        HTTP_ERROR("Error serving file: {}", e.what());
        serveStatusCodeImage(responseBuilder, HttpStatusCode::InternalServerError);
    }
}


void HttpRequestHandler::serveStatusCodeImage(HttpResponseBuilder& responseBuilder, HttpStatusCode statusCode) {
    try {
        // 
        std::string filepath = BASE_DIRECTORY + "/status/" + std::to_string(static_cast<int>(statusCode)) + ".jpg";
        std::string extension = std::filesystem::path(filepath).extension().string();

        // read file content
        std::vector<unsigned char> fileContent = loadFile(filepath);

        // 
        responseBuilder.setStatusCode(statusCode);
        responseBuilder.setHeader("Content-Type", getMimeType(extension));
        responseBuilder.setBody(fileContent);

        // 
        HTTP_INFO("Served status code image '{}'", filepath);
    }
    catch (const std::exception& e) {
        // Image can't be loaded, use plain text message.
        HTTP_ERROR("Status code image not found: {}", e.what());
        responseBuilder.setStatusCode(statusCode);
        responseBuilder.setHeader("Content-Type", "text/plain");
        responseBuilder.setBody(statusCode2str(statusCode));
    }
}


} // namespace http::
