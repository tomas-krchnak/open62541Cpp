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
#include "clientnodetree.h"

namespace Open62541 {


bool ClientNodeTree::addFolderNode(
    NodeId&             parent,
    const std::string&  name,
    NodeId&             newNode) {
    NodeId node(_nameSpace, 0);
    return  _client.addFolder(parent, name, node, newNode, _nameSpace);
}

//*****************************************************************************

bool ClientNodeTree::addValueNode(
    NodeId&             parent,
    const std::string&  name,
    NodeId&             newNode,
    const Variant&      val) {
    NodeId node(_nameSpace, 0);
    return   _client.addVariable(parent, name, val, node, newNode, _nameSpace);
}

} // namespace Open62541
