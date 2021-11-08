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
#include <open62541cpp/objects/UANodeTree.h>
#include <open62541cpp/servernodetree.h>

namespace Open62541 {

/*!
    \brief ServerNodeTree
    \param s
    \param parent
    \param ns
*/
    ServerNodeTree::ServerNodeTree(Server& s, NodeId& parent, int ns)
    : UANodeTree(parent)
    , _server(s)
    , _nameSpace(ns)
{
}

/*!
 * \brief ~ServerNodeTree
 */
ServerNodeTree::~ServerNodeTree() {}

/*!
    \brief addFolderNode
    \param parent
    \param s
    \return
*/
bool ServerNodeTree::addFolderNode(
    NodeId& parent, 
    const std::string& s, 
    NodeId& no)
{
    NodeId ni(_nameSpace, 0);
    return _server.addFolder(parent, s, ni, no, _nameSpace);
}
/*!
    \brief addValueNode
    \return
*/
bool ServerNodeTree::addValueNode(
    NodeId& parent, 
    const std::string& s, 
    NodeId& no, 
    Variant& v)
{
    NodeId ni(_nameSpace, 0);
    return _server.addVariable(parent, s, v, ni, no, nullptr, _nameSpace);
}
/*!
    \brief getValue
    \return
*/
bool ServerNodeTree::getValue(NodeId& n, Variant& v)
{
    return _server.readValue(n, v);
}
/*!
    \brief setValue
    \return
*/
bool ServerNodeTree::setValue(NodeId& n, Variant& v)
{
    _server.writeValue(n, v);
    return _server.lastOK();
}

} // namespace Open62541
