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
#include "open62541cpp/serverbrowser.h"
#include "open62541cpp/open62541server.h"

namespace Open62541 {

void ServerBrowser::browse(const UA_NodeId& start)
{
    list().clear();
    UA_Server_forEachChildNodeCall(
        obj().server(),  // UA_Server*
        start,           // parent node id.
        browseIter,      // callback used to iterate on the children nodes.
        (void*)this);    // handle used as browseIter()'s third argument, storing the gathered info of each children.
}
} // namespace Open62541
