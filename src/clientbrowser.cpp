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
#include <clientbrowser.h>

namespace Open62541 {

void ClientBrowser::browse(UA_NodeId start) {
    list().clear();
    if (auto pClient = obj().client())
        UA_Client_forEachChildNodeCall(
            pClient,        // UA_Client*
            start,          // parent node id.
            browseIter,     // callback used to iterate on the children nodes.
            (void*)this);   // handle used as browseIter()'s third argument, storing the gathered info of each children.
}

} // namespace Open62541
