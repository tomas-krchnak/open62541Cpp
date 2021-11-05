/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include <map>
#include <string>
#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {

    /**
     * @class NodeIdMap open62541objects.h
     * RAII map of name, UA_NodeId with the put method added.
     * @see UA_NodeId in open62541.h
     */
    class UA_EXPORT NodeIdMap : public std::map<std::string, UA_NodeId>
    {
    public:
        NodeIdMap() {}  // set of nodes not in a tree
        virtual ~NodeIdMap();
        void put(const UA_NodeId& node);
    };
} // namespace Open62541
