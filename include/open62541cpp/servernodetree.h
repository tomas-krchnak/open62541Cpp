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

#ifndef OPEN62541OBJECTS_H
#include <open62541cpp/open62541objects.h>
#endif
#include <open62541cpp/objects/Variant.h>
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/objects/UANodeTree.h>

namespace Open62541 {

/*!
    \brief The ServerNodeTree class
*/
class ServerNodeTree : public UANodeTree
{
    Server& _server;     // server
    int _nameSpace = 2;  // sname space index we create nodes in
public:
    /*!
        \brief setNameSpace
        \param i
        \return
    */
    void setNameSpace(int i) { _nameSpace = i; }
    /*!
        \brief nameSpace
        \return
    */
    int nameSpace() const { return _nameSpace; }

    /**
     * ServerNodeTree Constructor
     * @param server a reference to the server of the tree.
     * @param parent the root of the tree
     * @param idxNamespace where the nodes will reside. 2 by default.
     */
    ServerNodeTree(Server& s, NodeId& parent, int ns = 2);
    // client and server have different methods - TO DO unify client and server - and template
    // only deal with value nodes and folders - for now

    /*!
     * \brief ~ServerNodeTree
     */
    virtual ~ServerNodeTree();

    /**
     * Add a children Folder node in the server, thread-safely.
     * @param parent parent node
     * @param name of the folder node
     * @param[out] newNode receives new node if not null
     * @return true on success.
     */
    virtual bool addFolderNode(NodeId& parent, const std::string& s, NodeId& no);
    
    /**
     * Add a new variable node in the server, thread-safely.
     * @param parent specify the parent node containing the added node
     * @param name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param[out] newNode receives new node if not null
     * @return true on success.
     */
    virtual bool addValueNode(NodeId& parent, const std::string& s, NodeId& no, Variant& v);
    
    /**
     * Get the value of a given variable node.
     * @param node id of the node to read.
     * @param outValue return the value of the node.
     * @return true on success.
     */
    virtual bool getValue(NodeId& n, Variant& v);
    /*!
        \brief setValue
        \return
    */
    virtual bool setValue(NodeId& n, Variant& v);
};

}  // namespace Open62541
#endif /* SERVERNODETREE_H */
