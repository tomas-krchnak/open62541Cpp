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
#include "clientcachethread.h"

namespace Open62541 {

bool ClientCacheThread::start() {
    try {
        m_thread = std::thread([this] {
            m_running = true;
            while (m_running) {
                m_cache.process();
            }
        });
    }
    catch(...) {
        return false;
    }
    return true;
}

//*****************************************************************************

bool ClientCacheThread::stop() {
    m_running = false;
    m_thread.join();
    return true;
}

} // namespace Open62541
