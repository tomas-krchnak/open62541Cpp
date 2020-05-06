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

#include "open62541server.h"

namespace Open62541 {

/**
 * Class representing a tree of nodes for a server.
 * Wrap the server methods dealing with nodes.
 * Client and Server have different methods.
 * @todo unify Client and Server using template.
 * Only deal with value nodes and folders, for now.
 * The tree can only be expanded by adding folder or variable node.
 * Nodes value can be written and set.
 * Node removal isn't supported.
 */
class UA_EXPORT ServerNodeTree : public UANodeTree {
    Server& _server;        /**< server using the tree. */
    int     _nameSpace = 2; /**< name space index we create nodes in. */

public:
    /**
     * ServerNodeTree Constructor
     * @param server a reference to the server of the tree.
     * @param parent the root of the tree
     * @param idxNamespace where the nodes will reside. 2 by default.
     */
    ServerNodeTree(Server& server, NodeId& root, int idxNamespace = 2)
        : UANodeTree(root)
        , _server(server)
        , _nameSpace(idxNamespace)          {}

    virtual ~ServerNodeTree()               {}

    void    setNameSpace(int idxNamespace)  { _nameSpace = idxNamespace; }
    int     nameSpace()               const { return _nameSpace; }
    
    /**
     * Add a children Folder node in the server, thread-safely.
     * @param parent parent node
     * @param name of the folder node
     * @param[out] newNode receives new node if not null
     * @return true on success.
     */
    bool addFolderNode(
        const NodeId&       parent,
        const std::string&  name,
        NodeId&             newNode = NodeId::Null) override; // UANodeTree
    
    /**
     * Add a new variable node in the server, thread-safely.
     * @param parent specify the parent node containing the added node
     * @param name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param[out] newNode receives new node if not null
     * @return true on success.
     */
    bool addValueNode(
        const NodeId&       parent,
        const std::string&  name,
        const Variant&      value,
        NodeId&             newNode = NodeId::Null) override; // UANodeTree

    /**
     * Get the value of a given variable node.
     * @param node id of the node to read.
     * @param outValue return the value of the node.
     * @return true on success.
     */
    bool getValue(const NodeId& node, Variant& outValue) override {
        return _server.readValue(node, outValue);
    }

    /**
     * Set the value of a given variable node.
     * @param node id of the node to set.
     * @param val specify the new value of the node.
     * @return true on success.
     */
    bool setValue(NodeId& node, const Variant& val) override {
        return _server.writeValue(node, val);
    }
};

} // namespace Open62541

#endif // SERVERNODETREE_H
