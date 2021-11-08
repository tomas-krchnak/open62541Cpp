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
#include <open62541cpp/open62541server.h>
#endif
#ifndef BROWSABLETEMPLATE_H
#include <open62541cpp/objects/BrowsableTemplate.h>
#endif

namespace Open62541
{
// browsing object
/*!
    \brief The ServerBrowser class
    Browse a server node
*/
class UA_EXPORT ServerBrowser : public Browser<Server> {
public:
    ServerBrowser(Server& server)
        : Browser(server) {}
    
    /**
     * Reset and populate _list with the info of all the children node of a given node.
     * Info are the browse name, namespace, id and type, all stored in a BrowseItem. 
     * @param start id of the given node. Excluded from the list.
     * @see BrowseItem.
     */
    void browse(const UA_NodeId& start) {
        list().clear();
        UA_Server_forEachChildNodeCall(
            obj().server(), // UA_Server*
            start,          // parent node id.
            browseIter,     // callback used to iterate on the children nodes.
            (void*)this);   // handle used as browseIter()'s third argument, storing the gathered info of each children.
    }
};

} // namespace Open62541

#endif // SERVERBROWSER_H
