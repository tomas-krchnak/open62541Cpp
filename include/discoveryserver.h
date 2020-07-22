#ifndef DISCOVERYSERVER_H
#define DISCOVERYSERVER_H

/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "open62541objects.h"

namespace Open62541 {

/**
 * LDS (discovery server) object
 */
class UA_EXPORT DiscoveryServer {
    UA_ServerConfig*  m_pConfig = nullptr;
    UA_Server*        m_pServer = nullptr;
    UA_Boolean        m_running = true;     /**< Once the server running, setting this to false stops it. */
    void configure(int port, const std::string& url);

public:
    /**
     * Create and configure the server
     * @param port on which the server will listen
     * @param url server description
     */
    DiscoveryServer(int port, const std::string& url);

    virtual ~DiscoveryServer();

    /**
     * Start the server
     * Effectively calling UA_Server_run
     * @return true on success
     */
    bool run();
};

} // namespace Open62541

#endif // DISCOVERYSERVER_H
