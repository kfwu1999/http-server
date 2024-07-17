/**
 * \file src/request.cpp
 */

#include <exception>
#include <filesystem>
#include <mutex>

#include "request.h"
#include "response.h"
#include "file.h"
#include "log.h"


namespace http {


void HttpRequest::parse(const std::string& request) {
    // 
    std::istringstream requestStream(request);
    std::string line;

    // Parse the request line
    std::getline(requestStream, line);
    std::istringstream lineStream(line);
    lineStream >> this->method;
    lineStream >> this->path;
    lineStream >> this->version;
    HTTP_INFO("Parsed request line: method = {}, path = {}, version = {}", this->method, this->path, this->version);

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
            this->headers[headerName] = headerValue;

            HTTP_INFO("Parsed header: {}: {}", headerName, headerValue);
        }
    }

    //
    while (std::getline(requestStream, line)) {
        this->body += line;
    }
    HTTP_INFO("Parsed body of length {}", this->body.size());
}


std::string HttpRequestHandler::handleRequest(const std::string& request) {
    // 
    HTTP_TRACE("Handling HTTP request with length {}", request.length());

    // 
    HttpRequest httpRequest;
    httpRequest.parse(request);

    // 
    HttpResponseBuilder responseBuilder;

    // 
    try {
        if (httpRequest.method == "GET") {
            handleGetRequest(httpRequest, responseBuilder);
        }
        else {
            HTTP_ERROR("Unsupported HTTP method: {}", httpRequest.method);
            serveStatusCodeImage(responseBuilder, HttpStatusCode::BadRequest);
        }
    } catch (const std::exception& e) {
        HTTP_ERROR("Error handling request: {}", e.what());
        serveStatusCodeImage(responseBuilder, HttpStatusCode::BadRequest);
    }

    // 
    std::string response = responseBuilder.build();
    HTTP_INFO("Handled request, response length: {}", response.size());
    return response;
}


void HttpRequestHandler::handleGetRequest(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder) {
    // 
    HTTP_TRACE("Handling GET request for path '{}'", httpRequest.path);

    // 
    if (httpRequest.path == "/")
        httpRequest.path = "/home.html";

    // 
    if (httpRequest.path.substr(0, 5) == "/echo") {
        handleEcho(httpRequest, responseBuilder);
        HTTP_INFO("Responding to GET request for '/echo'");
    }
    else {
        serveStaticFile(httpRequest, responseBuilder);
    }
}


void HttpRequestHandler::handleEcho(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder) {
    // 
    responseBuilder.setStatusCode(HttpStatusCode::OK);
    responseBuilder.setHeader("Content-Type", "text/plain");

    // 
    std::string responseBody = "Echoing request details:\r\n";
    responseBody += "Method: " + httpRequest.method + "\r\n";
    responseBody += "Path: " + httpRequest.path + "\r\n";
    responseBody += "Version: " + httpRequest.version + "\r\n";
    responseBody += "Headers:\r\n";

    for (const auto& header : httpRequest.headers) {
        responseBody += "- " + header.first + ": " + header.second + "\r\n";
    }

    // 
    responseBody += "Body:\r\n" + httpRequest.body + "\r\n";
    responseBuilder.setBody(responseBody);
}


void HttpRequestHandler::serveStaticFile(HttpRequest& httpRequest, HttpResponseBuilder& responseBuilder) {
    // 
    try {
        // 
        std::string filepath = mapUrlToFilePath(httpRequest.path);
        std::string extension = std::filesystem::path(filepath).extension().string();

        // 
        std::vector<unsigned char> cacheContent;
        std::vector<unsigned char> fileContent;

        // minimize the critical section
        {
            std::lock_guard<std::mutex> lock(r_cacheMtx);
            cacheContent = r_cache.get(filepath);
        }

        // 
        if (cacheContent.empty()) {
            // 
            fileContent = loadFile(filepath);

            // minimize the critical section
            {
                std::lock_guard<std::mutex> lock(r_cacheMtx);
                r_cache.put(filepath, fileContent);
            }
            HTTP_INFO("Served static file '{}'", filepath);
        }
        else {
            fileContent = cacheContent;
            HTTP_INFO("Served static file from cache");
        }

        // 
        responseBuilder.setStatusCode(HttpStatusCode::OK);
        responseBuilder.setHeader("Content-Type", getMimeType(extension));
        responseBuilder.setBody(fileContent);
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


void HttpRequestHandler::serveStatusCodeImage(HttpResponseBuilder& responseBuilder, const HttpStatusCode& statusCode) {
    //
    try {
        // 
        std::string filepath = BASE_DIRECTORY + "/status/" + std::to_string(static_cast<int>(statusCode)) + ".jpg";
        std::string extension = std::filesystem::path(filepath).extension().string();

        // 
        std::vector<unsigned char> cacheContent;
        std::vector<unsigned char> fileContent;

        // minimize the critical section
        {
            std::lock_guard<std::mutex> lock(r_cacheMtx);
            cacheContent = r_cache.get(filepath);
        }

        // 
        if (cacheContent.empty()) {
            // 
            fileContent = loadFile(filepath);

            // minimize the critical section
            {
                std::lock_guard<std::mutex> lock(r_cacheMtx);
                r_cache.put(filepath, fileContent);
            }
            HTTP_INFO("Served status code image '{}'", filepath);
        }
        else {
            fileContent = cacheContent;
            HTTP_INFO("Served status code image from cache");
        }

        // 
        responseBuilder.setStatusCode(statusCode);
        responseBuilder.setHeader("Content-Type", getMimeType(extension));
        responseBuilder.setBody(fileContent);
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
