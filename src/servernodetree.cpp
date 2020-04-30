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
#include "servernodetree.h"

namespace Open62541 {

bool ServerNodeTree::addFolderNode(
    NodeId&             parent,
    const std::string&  name,
    NodeId&             outNewNode) {
    NodeId node(_nameSpace, 0);
    return _server.addFolder(parent, name, node, outNewNode, _nameSpace);
}

//*****************************************************************************

bool ServerNodeTree::addValueNode(
    NodeId&             parent,
    const std::string&  name,
    NodeId&             outNewNode,
    const Variant&      val) {
    NodeId node(_nameSpace, 0);
    return _server.addVariable(
        parent,
        name,
        val,
        node,
        outNewNode,
        nullptr,
        _nameSpace);
}

} // namespace Open62541
