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

namespace Open62541 {

/**
 * The ServerNodeTree class
 */
class UA_EXPORT ServerNodeTree : public UANodeTree {
    Server &_server;    /**< server */
    int _nameSpace = 2; /**< name space index we create nodes in */

public:
    /**
     * setNameSpace
     * @param idxNamespace
     * @return 
     */
    void setNameSpace(int idxNamespace) {
        _nameSpace = idxNamespace;
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
     * client and server have different methods
     * @todo unify client and server - and template
     * only deal with value nodes and folders - for now
     * @param server
     * @param parent
     * @param idxNamespace
     */
    ServerNodeTree(Server& server, NodeId& parent, int idxNamespace = 2);

    /**
     * ~ServerNodeTree
     */
    virtual ~ServerNodeTree();

    /**
     * addFolderNode
     * @param parent
     * @param name
     * @param node
     * @return true on success.
     */
    bool addFolderNode(NodeId& parent, const std::string& name, NodeId& node) override;

    /**
     * addValueNode
     * @return true on success.
     */
    bool addValueNode(
        NodeId&             parent,
        const std::string&  name,
        NodeId&             node,
        const Variant&      val) override;

    /**
     * Get the value of a given variable node.
     * @return true on success.
     */
    bool getValue(const NodeId& node, Variant& val) override;

    /**
     * Set the value of a given variable node.
     * @return true on success.
     */
    bool setValue(NodeId& node, const Variant& val) override;
};

} // namespace Open62541

#endif // SERVERNODETREE_H
