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
#include <open62541server.h>

/**
 * ServerNodeTree
 * @param s
 * @param parent
 * @param ns
 */
Open62541::ServerNodeTree::ServerNodeTree(Server &s, NodeId &parent, int ns)
    : UANodeTree(parent), _server(s), _nameSpace(ns)
{

}

/**
 * ~ServerNodeTree
 */
Open62541::ServerNodeTree::~ServerNodeTree()
{

}

/**
 * addFolderNode
 * @param parent
 * @param s
 * @return 
 */
bool Open62541::ServerNodeTree::addFolderNode(NodeId &parent, const std::string &s, NodeId &no) {
    NodeId ni(_nameSpace, 0);
    return _server.addFolder(parent, s, ni, no, _nameSpace);
}

/**
 * addValueNode
 * @return 
 */
bool Open62541::ServerNodeTree::addValueNode(NodeId &parent, const std::string &s, NodeId &no, Variant &v) {
    NodeId ni(_nameSpace, 0);
    return _server.addVariable(parent, s, v, ni, no, nullptr,_nameSpace);
}

/**
 * getValue
 * @return 
 */
bool Open62541::ServerNodeTree::getValue(NodeId &n, Variant &v) {
    return _server.readValue(n, v);
}

/**
 * setValue
 * @return 
 */
bool Open62541::ServerNodeTree::setValue(NodeId &n, Variant &v) {
    _server.writeValue(n, v);
    return _server.lastOK();
}
