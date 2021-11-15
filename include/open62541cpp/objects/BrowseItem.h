/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BROWSEITEM_H
#define BROWSEITEM_H

#include <string>
#include "open62541/types.h"

namespace Open62541 {

    struct UA_EXPORT BrowseItem {
        std::string name;  /**< the node browse name */
        int nameSpace = 0; /**< the node namespace index */
        UA_NodeId nodeId;  /**< the node id */
        UA_NodeId type;    /**< the node's node type */

        BrowseItem(const std::string& t_name, int t_nsIdx, UA_NodeId t_node, UA_NodeId t_type)
            : name(t_name)
            , nameSpace(t_nsIdx)
            , nodeId(t_node)
            , type(t_type)
        {
        }

        BrowseItem(const BrowseItem& item)
            : name(item.name)
            , nameSpace(item.nameSpace)
            , nodeId(item.nodeId)
            , type(item.type)
        {
        }
    };

}  // namespace Open62541


#endif /* BROWSEITEM_H */
