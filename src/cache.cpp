/**
 * \file src/cache.c
 */

#include <chrono>

#include "cache.h"


namespace http {


void LRUCache::put(const std::string& path, const std::vector<unsigned char>& body) {
    // 
    auto it = m_lookup.find(path);

    // found, update body and move to front
    if (it != m_lookup.end()) {
        it->second->body = body;
        it->second->createAt = std::chrono::system_clock::now();   // update timestamp of createAt
        m_list.splice(m_list.begin(), m_list, it->second);
    }
    // not found, insert path and body
    else {
        // remove least recently used when capacity exceeds limit
        if (m_list.size() >= m_capacity) {
            auto leastRecentlyUsedIt = m_lookup.find(m_list.back().path);
            m_lookup.erase(leastRecentlyUsedIt);
            m_list.pop_back();
        }

        // insert to recently used
        m_list.push_front({path, body, std::chrono::system_clock::now()});
        m_lookup[path] = m_list.begin();
    }
}


std::vector<unsigned char> LRUCache::get(const std::string& path) {
    // 
    auto it = m_lookup.find(path);

    // not found
    if (it == m_lookup.end()) {
        return {};
    }

    // move to recently used
    m_list.splice(m_list.begin(), m_list, it->second);

    // return the body
    return it->second->body;
}


std::vector<unsigned char> LRUCache::getOrDeleteExpired(const std::string& path) {
    // 
    auto it = m_lookup.find(path);

    // not found
    if (it == m_lookup.end()) {
        return {};
    }

    // 
    auto duration = std::chrono::system_clock::now() - it->second->createAt;
    if (duration >= m_durationThreshInMin) {
        auto itList = it->second;
        m_lookup.erase(it);
        m_list.erase(itList);
        // HTTP_DEBUG("Delete expired cache content.");
        return {};
    }
    // HTTP_DEBUG("Not expired");

    // move to recently used
    m_list.splice(m_list.begin(), m_list, it->second);

    // return the body
    return it->second->body;
}


} // namespace http::
