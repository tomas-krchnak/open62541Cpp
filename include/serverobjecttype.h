/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef SERVEROBJECTTYPE_H
#define SERVEROBJECTTYPE_H

#include "open62541server.h"

namespace Open62541 {

/**
 * The ServerObjectType class
 * Object type handling class
 * this is a factory for object type - operates on a server instance
 * The NodeContext is the node life cycle manager
 */
class UA_EXPORT ServerObjectType {
    Server&     _server;        /**< server of the Type */
    std::string _name;          /**< name of the Type */
    NodeId      _typeId;        /**< node of the Type */
    int         _nameSpace = 2; /**< namespace index of the Type. 2 by default. */

public:
    ServerObjectType(Server& server, const std::string& name)
        : _server(server)
        , _name(name)           {}
    virtual ~ServerObjectType() = default;

    void    setNameSpace(int i) { _nameSpace = i; }
    int     nameSpace()   const { return _nameSpace; }
    Server& server()            { return _server; }
    NodeId& typeId()            { return _typeId; }
    const std::string& name()   { return _name; }

    /**
     * Add Base Object Type
     * Creates an object type node with the BaseObject and HasSubType traits.
     * It means this is the root node of an object hierarchy
     * @param[in] name specify the display name of the object type
     * @param[in,out] requestNodeId specify if a nodeId is already dedicated to hold the definition or if the nodeid must be created and returned.
     *                if NodeId::Null a node is created and returned.
     * @param context
     * @return true on success, false otherwise
     */
    bool addBaseObjectType(
        const std::string&  name,
        NodeId&             requestNodeId   = NodeId::Null,
        NodeContext*        context         = nullptr);

    /**
     * Add a Variable node to a parent object type node.
     * @param name of the created Variable node
     * @param parent specifies the node of the object type parent node
     * @param nodeId 
     * @param mandatory
     * @return true on success, false otherwise
     */
    template<typename T>
    bool addObjectTypeVariable(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             outNewNodeId    = NodeId::Null,
        NodeContext*        context         = nullptr,
        NodeId&             requestNodeId   = NodeId::Null, // usually want auto generated ids
        bool                mandatory       = true) {

        T a{};
        Variant value(a);

        VariableAttributes var_attr;
        var_attr.setDefault();
        var_attr.setDisplayName(name);
        var_attr.setDescription(name);
        var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        var_attr.setValue(value);
        var_attr.get().dataType = value.get().type->typeId;

        NodeId newNode;
        newNode.notNull();

        if (!_server.addVariableNode(
                requestNodeId,
                parent,
                NodeId::HasComponent,
                QualifiedName(_nameSpace, name.c_str()),
                NodeId::BaseDataVariableType,
                var_attr,
                newNode,
                context)){
            UAPRINTLASTERROR(_server.lastError())
            return false;
        }

        if (mandatory)
            return setMandatory(newNode);

        if (!outNewNodeId.isNull())
            outNewNodeId = newNode;

        return true;
    }
    
    /**
     * addHistoricalObjectTypeVariable
     * @param n
     * @param parent
     * @param nodeiD
     * @param mandatory
     * @return true on success, false otherwise
     */
    template<typename T>
    bool addHistoricalObjectTypeVariable(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             outNewNodeId    = NodeId::Null,
        NodeContext*        context         = nullptr,
        NodeId&             requestNodeId   = NodeId::Null, // usually want auto generated ids
        bool                mandatory       = true) {

        T a{};
        Variant value(a);

        VariableAttributes var_attr;
        var_attr.setDefault();
        var_attr.setDisplayName(name);
        var_attr.setDescription(name);
        var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYREAD;
        var_attr.setValue(value);
        var_attr.get().dataType = value.get().type->typeId;
        var_attr.get().historizing = true;

        NodeId newNode;
        newNode.notNull();

        if (!_server.addVariableNode(
                requestNodeId,
                parent,
                NodeId::HasComponent,
                QualifiedName(_nameSpace, name.c_str()),
                NodeId::BaseDataVariableType,
                var_attr,
                newNode,
                context)) {
            UAPRINTLASTERROR(_server.lastError())
            return false;
        }

        if (mandatory)
            return setMandatory(newNode);

        if (!outNewNodeId.isNull())
            outNewNodeId = newNode;

        return true;
    }

    /**
    * addObjectTypeFolder
    * @param n
    * @param parent
    * @param nodeiD
    * @param mandatory
    * @return true on success, false otherwise
    */
    bool addObjectTypeFolder(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             outNewNodeId    = NodeId::Null,
        NodeId&             requestNodeId   = NodeId::Null,
        bool                mandatory       = true);
    
    /**
     * Set a node as Mandatory
     * Add the Mandatory rule to a node
     * @param node specifies the mandatory node
     * @return true on success, false otherwise
     */
    bool setMandatory(NodeId& node);
    
    /**
     * Add a Derived Object Type an object hierarchy
     * Creates an object type node with the HasSubType traits.
     * It means this is a derived node of an object hierarchy
     * @param[in] name specify the display name of the object type
     * @param[in] parent specifies the parent object type node containing it
     * @param[in] nodeId specifies the node with the definition
     * @param[in,out] requestNodeId specify if a nodeId is already dedicated to hold the definition or if the nodeid must be created and returned.
     *                if NodeId::Null a node is created and returned.
     * @param context
     * @return true on success, false otherwise
     */
    bool addDerivedObjectType(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             nodeId          = NodeId::Null,
        NodeId&             requestNodeId   = NodeId::Null,
        NodeContext*        context         = nullptr);

    /**
     * Add children nodes to a given node.
     * @param parent the id of the node to modify.
     * @return true on success, false otherwise
     */
    virtual bool addChildren(NodeId& parent)    { return true; }

    /**
     * addType
     * @param nodeId specify the base node of the type
     * @return true on success, false otherwise
     */
    virtual bool addType(NodeId& nodeId);

    /**
     * Append a node to a parent as a derived object type
     * @param parent the parent node object type
     * @param nodeId the appended node
     * @param requestNodeId
     * @return true on success, false otherwise
     */
    virtual bool append(
        NodeId& parent,
        NodeId& nodeId,
        NodeId& requestNodeId = NodeId::Null); // derived type

    /**
     * addInstance
     * @param n
     * @param parent
     * @param nodeId
     * @return true on success, false otherwise
     */
    virtual bool addInstance(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             outNewNodeId = NodeId::Null,
        NodeId&             requestNodeId = NodeId::Null,
        NodeContext*        context       = nullptr);
};

} // namespace Open62541

#endif // SERVEROBJECTTYPE_H
