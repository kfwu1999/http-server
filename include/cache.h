/**
 * \file include/cache.h
 */

#pragma once

#ifndef CACHE_H_
#define CACHE_H_

#include <string>
#include <list>
#include <unordered_map>
#include <vector>


namespace http {

/**
 */
class LRUCache {
private:
    struct CacheEntry {
        std::string path;
        std::vector<unsigned char> body;
    };

public:
    /**
     * \brief Initialize LRUCache with given capacity.
     */
    explicit LRUCache(std::size_t capacity) : m_capacity(capacity) {}

    /**
     * \brief Put file content in cache.
     *
     * \param path: The path of the file.
     * \param body: The content of the file.
     */
    void put(const std::string& path, const std::vector<unsigned char>& body);

    /**
     * \brief Get the content of a file from the cache.
     *
     * \param path: The path of the file.
     * \return The content of the file, or an empty string if not found.
     */
    std::vector<unsigned char> get(const std::string& path);

private:
    std::size_t m_capacity;
    std::list<CacheEntry> m_list;
    std::unordered_map<std::string, std::list<CacheEntry>::iterator> m_lookup;
};


} // namespace http::

#endif // CACHE_H_
