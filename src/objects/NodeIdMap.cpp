
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/NodeIdMap.h>
#include <open62541cpp/objects/StringUtils.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {
NodeIdMap::~NodeIdMap()
{
    for (auto& i : *this) {
        UA_NodeId_deleteMembers(&i.second);  // delete node data
    }
    clear();
}

//*****************************************************************************

void NodeIdMap::put(const UA_NodeId& node)
{
    UA_NodeId copy;  // deep copy
    UA_NodeId_init(&copy);
    UA_NodeId_copy(&node, &copy);
    const std::string s = toString(copy);
    insert(std::pair<std::string, UA_NodeId>(s, copy));
}
}  // namespace Open62541
