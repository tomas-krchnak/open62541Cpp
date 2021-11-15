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
#ifndef CLIENTCACHETHREAD_H
#define CLIENTCACHETHREAD_H

#include <thread>

#ifndef CLIENTCACHE_H
#include <open62541cpp/clientcache.h>
#endif

namespace Open62541 {

/**
 * Class periodically processing a given list of clients.
 * The period is as fast as possible and depend only on the number of clients
 * and the duration of their process() call.
 */
class ClientCacheThread {
    ClientCache&    m_cache;
    std::thread     m_thread;
    bool            m_running = false;

public:
    /**
     * ClientCacheThread Constructor
     * @param cache a reference to a cache of clients to process periodically.
     */
    ClientCacheThread(ClientCache& cache)
        : m_cache(cache) {}

    /**
     * start the periodical client cache processing
     * @return true on success 
     */
    bool start();

    /**
     * stop the client cache periodical processing.
     * @return always true 
     */
    bool stop();

    /**
     * Accessor for the client cache.
     * @return a non-const reference to the client cache.
     */
    ClientCache& cache() { return m_cache; }
};

} // namespace Open62541

#endif /* CLIENTCACHETHREAD_H */
