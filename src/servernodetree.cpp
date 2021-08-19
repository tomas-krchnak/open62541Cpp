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
#include <open62541cpp/servernodetree.h>
#include <open62541cpp/open62541server.h>

namespace Open62541 {

bool ServerNodeTree::addFolderNode(
    const NodeId&       parent,
    const std::string&  name,
    NodeId&             outNewNode /*= NodeId::Null*/) {
    NodeId node(m_nameSpace, 0);
    return m_server.addFolder(parent, name, node, outNewNode, m_nameSpace);
}

//*****************************************************************************

bool ServerNodeTree::addValueNode(
    const NodeId&       parent,
    const std::string&  name,
    const Variant&      val,
    NodeId&             outNewNode /*= NodeId::Null*/) {
    NodeId node(m_nameSpace, 0);
    return m_server.addVariable(
        parent,
        name,
        val,
        node,
        outNewNode,
        nullptr,
        m_nameSpace);
}

} // namespace Open62541
