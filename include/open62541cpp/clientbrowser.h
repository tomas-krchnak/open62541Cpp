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
#ifndef CLIENTBROWSER_H
#define CLIENTBROWSER_H
#include <open62541cpp/open62541client.h>
namespace Open62541
{

#ifndef OPEN62541CLIENT_H
#include "open62541client.h"
#endif

namespace Open62541 {

/**
 * The ClientBrowser class
 * Browse nodes helper.
 */
class ClientBrowser : public Browser<Client> {
public:
    /**
     * ClientBrowser Ctor
     * @param client the client connection
     */
    ClientBrowser(Client& client)
        : Browser(client) {}

    /**
     * Reset and populate _list with the info of all the children node of a given node.
     * Info are the browse name, namespace, id and type, all stored in a BrowseItem. 
     * @param start id of the given node. Excluded from the list.
     * @see BrowseItem.
     */
    void browse(const UA_NodeId& start);
};

}// namespace Open62541

#endif // CLIENTBROWSER_H
