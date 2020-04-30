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
#include "clientcache.h"

namespace Open62541 {

ClientRef& ClientCache::add(const std::string& endpoint) {
    if (_cache.find(endpoint) == _cache.end()) {
        _cache[endpoint] = std::make_shared<Client>();
    }
    return _cache[endpoint];
}

//*****************************************************************************

void ClientCache::remove(const std::string& endpoint) {
    if (auto a = find(endpoint)) {
        a->disconnect();
    }   
    _cache.erase(endpoint);
}

//*****************************************************************************

Client* ClientCache::find(const std::string& endpoint) {
    if (_cache.find(endpoint) != _cache.end()) {
        return _cache[endpoint].get();
    }
    return nullptr;
}

//*****************************************************************************

void ClientCache::process() {
    for (auto& client : _cache) {
        if (client.second)
            client.second->process();
    }
}

} // namespace Open62541
