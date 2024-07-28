/**
 */

#include <fstream>
#include <filesystem>

#include "file.h"
#include "log.h"


namespace http {


std::string mapUrlToFilePath(const std::string& urlPath) {
    std::string baseDir = BASE_DIRECTORY;
    std::filesystem::path filePath = std::filesystem::canonical(baseDir + urlPath);
    return filePath.string();
}


std::vector<unsigned char> loadFile(std::string const& filepath) {
    // 
    std::ifstream file(filepath, std::ios::binary);

    // 
    if (!file) {
        HTTP_ERROR("Failed to open file: {}", filepath);
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    std::vector<unsigned char> result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // 
    return result;
}


std::string getMimeType(const std::string& extension) {
    // 
    static std::unordered_map<std::string, std::string> mimeTypes = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".gif", "image/gif"},
    };

    // 
    auto it = mimeTypes.find(extension);
    if (it != mimeTypes.end())
        return mimeTypes[extension];
    return DEFAULT_MIME_TYPE;
}


} // namespace http::
