/**
 * \file include/file.h
 */

#pragma once


#ifndef FILE_H_
#define FILE_H_

#include <vector>
#include <string>

#define BASE_DIRECTORY    std::string("../files")
#define DEFAULT_MIME_TYPE std::string("application/octet-stream")

namespace http {


/**
 * \brief Maps a URL path to a filesystem path.
 *
 * \param urlPath: The URL path to be mapped.
 * \return The corresponding filesystem path.
 */
std::string mapUrlToFilePath(const std::string& urlPath);


/**
 * \brief Loads the contents of a file into a vector of unsigned char.
 *
 * \param filepath: The path of the file to be loaded.
 * \return A vector of unsigned char containing the file's contents.
 * \throws std::runtime_error if the file can't be opened or read.
 */
std::vector<unsigned char> loadFile(const std::string& filepath);

/**
 * \brief Determines the MIME type based on the file extension.
 *
 * \param extension: The file extension for which to determine the MIME type.
 * \return The corresponding MIME type as a string.
 * \ref: https://github.com/lasselukkari/MimeTypes
 */
std::string getMimeType(const std::string& extension);


} // namespace http::

#endif // FILE_H_
