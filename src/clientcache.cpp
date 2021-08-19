/*
 * Copyright (C) 2017 -  B. J. Hill
 *
 * This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
 * redistribute it and/or modify it under the terms of the Mozilla Public
 * License v2.0 as stated in the LICENSE file provided with open62541.
 *
 * open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 */
#include <open62541cpp/clientcache.h>

namespace Open62541 {

ClientRef& ClientCache::add(const std::string& endpoint) {
    if (m_cache.find(endpoint) == m_cache.end()) {
        m_cache[endpoint] = std::make_shared<Client>();
    }
    return m_cache[endpoint];
}

//*****************************************************************************

void ClientCache::remove(const std::string& endpoint) {
    if (auto a = find(endpoint)) {
        a->disconnect();
    }   
    m_cache.erase(endpoint);
}

//*****************************************************************************

Client* ClientCache::find(const std::string& endpoint) {
    if (m_cache.find(endpoint) != m_cache.end()) {
        return m_cache[endpoint].get();
    }
    return nullptr;
}

//*****************************************************************************

void ClientCache::process() {
    for (auto& client : m_cache) {
        if (client.second)
            client.second->process();
    }
}

} // namespace Open62541
