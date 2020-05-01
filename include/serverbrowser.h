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

#ifndef SERVERBROWSER_H
#define SERVERBROWSER_H

#ifndef OPEN62541SERVER_H
#include <open62541server.h>
#endif

namespace Open62541 {

/**
 * Browse a server node
*/
class UA_EXPORT ServerBrowser : public Browser<Server> {
public:
    ServerBrowser(Server& server)
        : Browser(server) {}

    /**
     * browse iterate over each children nodes of a given node
     * @param start specify the starting node id to browse from
     */
    void browse(UA_NodeId start) {
        list().clear();
        UA_Server_forEachChildNodeCall(
            obj().server(), // UA_Server*
            start,          // parent node id
            browseIter,     // callback used to iterate on the children nodes
            (void*)this);   // handle used as browseIter()'s third argument. Will be cast to BrowserBase*
    }
};

} // namespace Open62541

#endif // SERVERBROWSER_H
