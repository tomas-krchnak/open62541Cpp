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
#ifndef CLIENTCACHE_H
#define CLIENTCACHE_H

#ifndef OPEN62541CLIENT_H
#include <open62541cpp/open62541client.h>
#endif

namespace Open62541 {

/**
 * ClientRef
 */
typedef std::shared_ptr<Client>          ClientRef;
typedef std::map<std::string, ClientRef> ClientMap;

/**
 * The ClientCache class
 */
class ClientCache {
    ClientMap m_cache;  /**< Cache / Dictionary of Client objects.
                             these are shared pointers so can be safely copied */
public:
            ClientCache()  = default;
    virtual ~ClientCache() = default;

    /**
     * Add an endpoint to the cache map.
     * If already in the cache, it isn't added.
     * @param endpoint name of the endpoint to add.
     * @return a reference to the client interface of the endpoint
     */
    ClientRef& add(const std::string& endpoint);

    /**
     * Remove the client associated with the given endpoint
     * @param endpoint name of client to remove
     */
    void remove(const std::string& endpoint);

    /**
     * Find a client by its name.
     * @param endpoint name of client to find
     * @return pointer to found client, nullptr otherwise.
     */
    Client* find(const std::string& endpoint);

    /**
     * Call the process method of each client in cache.
     * This method need to be specialized to do anything.
     * Periodic processing interface.
     */
    void process();
}; // class ClientCache

} // namespace Open62541

#endif // CLIENTCACHE_H
