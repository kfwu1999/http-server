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
#include <chrono>


namespace http {

/**
 * \brief A Least-Recently-Used (LRU) cache.
 *
 * By default, the cache entries will expire 1 minute after creation or update 
 * if accessed by using `getOrDeleteExpired`
 */
class LRUCache {
private:
    struct CacheEntry {
        std::string path;
        std::vector<unsigned char> body;
        std::chrono::time_point<std::chrono::system_clock> createAt;
    };

private:
    static constexpr std::chrono::minutes m_durationThreshInMin = std::chrono::minutes(1);

public:
    /**
     * \brief Initialize LRUCache with given capacity.
     *
     * If the capacity is less than 1, it defaults to 10.
     */
    explicit LRUCache(std::size_t capacity) : m_capacity(capacity > 0 ? capacity : 10) {}

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
     * \return The content of the file, or an empty vector if not found.
     */
    std::vector<unsigned char> get(const std::string& path);

    /**
     * \brief Get the content of a file from the cache. If it's expired, delete it
     *
     * If the path is found but the content has expired, the entry will be removed 
     * and return a empty vector. 
     * If the path is not found, return a empty vector.
     *
     * \param path: The path of the file.
     * \return The content of the file, or an empty vector if not found.
     */
    std::vector<unsigned char> getOrDeleteExpired(const std::string& path);


private:
    std::size_t m_capacity;
    std::list<CacheEntry> m_list;
    std::unordered_map<std::string, std::list<CacheEntry>::iterator> m_lookup;
};


} // namespace http::

#endif // CACHE_H_
