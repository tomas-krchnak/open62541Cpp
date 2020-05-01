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
     * @param[in,out] requestedNewNodeId assigned node id or NodeId::Null for auto assign.
     * @param context
     * @return true on success, false otherwise
     */
    bool addBaseObjectType(
        const std::string&  name,
        NodeId&             requestNodeId   = NodeId::Null,
        NodeContext*        context         = nullptr);

    /**
     * Add a Variable node to a parent object type node.
     * @param T specify the UA_ built-in type.
     * @param name of the new Type node
     * @param parent of the new node.
     * @param[out] outNewNodeId receives new node if not null.
     * @param context customize how the node will be created if not null.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param mandatory specify if the node is mandatory in instances.
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
    * Add a Historical Variable node to a parent object type node.
    * @param T specify the UA_ built-in type.
    * @param name of the new Type node
    * @param parent of the new node.
    * @param[out] outNewNodeId receives new node if not null.
    * @param context customize how the node will be created if not null.
    * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
    * @param mandatory specify if the node is mandatory in instances.
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
    * Add a folder node to a parent object type node.
    * @param name of the new Type node
    * @param parent of the new node.
    * @param[out] outNewNodeId receives new node if not null.
    * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
    * @param mandatory specify if the node is mandatory in instances.
    * @return true on success, false otherwise
    */
    bool addObjectTypeFolder(
        const std::string&  name,
        NodeId&             parent,
        NodeId&             outNewNodeId    = NodeId::Null,
        NodeId&             requestNodeId   = NodeId::Null,
        bool                mandatory       = true);
    
    /**
     * Set a node as Mandatory in the object instances, by adding the Mandatory rule in it.
     * If the node isn't explicitly constructed,
     * it will be created with default value.
     * @param node specifies the id of the mandatory node
     * @return true on success, false otherwise
     */
    bool setMandatory(NodeId& node);
    
    /**
     * Add a Derived Object Type in an object hierarchy
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
     * Hook to customize the addition of children node to the object type node.
     * Do nothing by default.
     * @param parent the id of the node to modify.
     * @return true on success, false otherwise
     */
    virtual bool addChildren(NodeId& parent)    { return true; }

    /**
     * Add the object type and its children.
     * @param nodeId specify the id of the base node of the type
     * @return true on success, false otherwise
     */
    virtual bool addType(NodeId& nodeId);

    /**
     * Append a node to a parent as a derived object type.
     * The derived object type'children are added as well.
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
     * Add an instance of this object type.
    * @param name of the instance.
    * @param parent of the instance base node.
    * @param[out] outNewNodeId receives new node if not null.
    * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
    * @return true on success, false otherwise.
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
