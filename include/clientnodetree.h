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
#ifndef CLIENTNODETREE_H
#define CLIENTNODETREE_H
#include "open62541client.h"
namespace Open62541
{

/**
 * The ServerNodeTree class
 */
class UA_EXPORT ClientNodeTree : public UANodeTree {
    Client &_client;  // client
    int _nameSpace = 2; // name space index we create nodes in

public:
    /**
     * ServerNodeTree
     * @param s
     * @param parent
     * @param ns
     */
    ClientNodeTree(Client &s, NodeId &parent, int ns = 2)
        : UANodeTree(parent),
            _client(s),
            _nameSpace(ns) {
        //std::cerr << __FUNCTION__ << " parent " << toString(parent) << std::endl;
    }

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
     * browse
     * @return 
     */
    bool browse() {
        return _client.browseTree(root().data(), *this); // load the tree
    }

    // client and server have different methods - TO DO unify client and server - and template
    // only deal with value nodes and folders - for now
    /**
     * addFolderNode
     * @param parent
     * @param s
     * @return 
     */
    virtual bool addFolderNode(NodeId &parent, const std::string &s, NodeId &no) {
        NodeId ni(_nameSpace, 0);
        return  _client.addFolder(parent, s, ni, no, _nameSpace);
    }
    /**
     * addValueNode
     * @return 
     */
    virtual bool addValueNode(NodeId &parent, const std::string &s, NodeId &no, Variant &v) {
        NodeId ni(_nameSpace, 0);
        return   _client.addVariable(parent, s, v, ni, no, _nameSpace);
    }
    /**
     * getValue
     * @return 
     */
    virtual bool getValue(NodeId &n, Variant &v) {
        return _client.variable(n, v);
    }
    /**
     * setValue
     * @return 
     */
    virtual bool setValue(NodeId &n, Variant &v) {
        return  _client.setVariable(n, v);
    }
};

} // namespace Open62541

#endif // CLIENTNODETREE_H
