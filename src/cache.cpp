/**
 * \file src/cache.c
 */

#include "cache.h"


namespace http {


void LRUCache::put(const std::string& path, const std::string& body) {
    // 
    auto it = m_lookup.find(path);

    // found, update body
    if (it != m_lookup.end()) {
        (it->second)->body = body;
        m_list.splice(m_list.begin(), m_list, it->second);
    }
    // not found, insert path and body
    else {
        // remove least recently used when capacity exceed limit
        if (m_list.size() >= m_capacity) {
            auto leastRecentlyUsedIt = m_lookup.find(m_list.back().path);
            m_lookup.erase(leastRecentlyUsedIt);
            m_list.pop_back();
        }

        // insert to recently used
        m_list.push_front({path, body});
        m_lookup[path] = m_list.begin();
    }
}


std::string LRUCache::get(const std::string& path) {
    // 
    auto it = m_lookup.find(path);

    // 
    if (it == m_lookup.end()) {
        return "";
    }

    // move to recently used
    m_list.splice(m_list.begin(), m_list, it->second);

    // 
    return (it->second)->body;
}


} // namespace http::
