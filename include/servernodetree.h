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
#ifndef SERVERNODETREE_H
#define SERVERNODETREE_H
#include "open62541objects.h"
#include "open62541server.h"
namespace Open62541
{

/**
 * The ServerNodeTree class
 */
class UA_EXPORT ServerNodeTree : public UANodeTree {
    Server &_server;  // server
    int _nameSpace = 2; // sname space index we create nodes in
public:

    /**
     * setNameSpace
     * @param i
     * @return 
     */
    void setNameSpace(int i) {
        _nameSpace = i;
    }
    /**
     * nameSpace
     * @return 
     */
    int nameSpace() const {
        return _nameSpace;
    }
    /**
     * ServerNodeTree
     * @param s
     * @param parent
     * @param ns
     */
    ServerNodeTree(Server &s, NodeId &parent, int ns = 2);
    // client and server have different methods - TO DO unify client and server - and template
    // only deal with value nodes and folders - for now

    /**
     * ~ServerNodeTree
     */
    virtual ~ServerNodeTree();
    /**
     * addFolderNode
     * @param parent
     * @param s
     * @return 
     */
    virtual bool addFolderNode(NodeId &parent, const std::string &s, NodeId &no);
    /**
     * addValueNode
     * @return 
     */
    virtual bool addValueNode(NodeId &parent, const std::string &s, NodeId &no, Variant &v);
    /**
     * getValue
     * @return 
     */
    virtual bool getValue(NodeId &n, Variant &v);
    /**
     * setValue
     * @return 
     */
    virtual bool setValue(NodeId &n, Variant &v);
};


} // namespace Open62541
#endif // SERVERNODETREE_H
