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
#include <open62541cpp/open62541objects.h>
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/nodecontext.h>

namespace Open62541 {

/**
 * The ServerObjectType class
 * Object type handling class
 * this is a factory for object type - operates on a server instance
 * The NodeContext is the node life cycle manager
 */
class UA_EXPORT ServerObjectType {
    Server&     m_server;         /**< server of the Type */
    std::string m_name;           /**< name of the Type */
    NodeId      m_typeId;         /**< node of the Type */
    int         m_nameSpace = 2;  /**< namespace index of the Type. 2 by default. */

public:
    ServerObjectType(Server& server, const std::string& name)
        : m_server(server)
        , m_name(name)           {}
    virtual ~ServerObjectType() = default;

    ServerObjectType&   setNameSpace(int i) { m_nameSpace = i; return *this; }
    int                 nameSpace()   const { return m_nameSpace; }
    Server&             server()            { return m_server; }
    NodeId&             typeId()            { return m_typeId; }
    const std::string&  name()              { return m_name; }

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
        const NodeId&       requestNodeId   = NodeId::Null,
        NodeContext*        context         = nullptr);

    /*!
    \brief addObjectTypeVariable
    \param n
    \param parent
    \param nodeiD
    \param mandatory
    \return
*/
    template <typename T>
    bool addObjectTypeVariable(const std::string& n,
                               const NodeId& parent,
                               NodeId& nodeId              = NodeId::Null,
                               NodeContext* context        = nullptr,
                               const NodeId& requestNodeId = NodeId::Null,  // usually want auto generated ids
                               bool mandatory              = true)
    {
        T a{};
        Variant value(a);
        //
        VariableAttributes var_attr;
        var_attr.setDefault();
        var_attr.setDisplayName(n);
        var_attr.setDescription(n);
        var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        var_attr.setValue(value);
        var_attr.get().dataType = value.get().type->typeId;
        //
        QualifiedName qn(m_nameSpace, n.c_str());
        //
        NodeId newNode;
        newNode.notNull();
        //
        if (m_server.addVariableNode(requestNodeId,
                                     parent,
                                     NodeId::HasComponent,
                                     qn,
                                     NodeId::BaseDataVariableType,
                                     var_attr,
                                     newNode,
                                     context)) {
            if (mandatory) {
                return m_server.addReference(newNode,
                                             NodeId::HasModellingRule,
                                             ExpandedNodeId::ModellingRuleMandatory,
                                             true);
            }
            if (!nodeId.isNull())
                nodeId = newNode;
            return true;
        }
        UAPRINTLASTERROR(m_server.lastError())
        return false;
    }

    //=========feat:addObjectTypeArrayVariable============//
    template <typename T, size_t size_array>
    bool addObjectTypeArrayVariable(const std::string& n,
                                    const NodeId& parent,
                                    NodeId& nodeId              = NodeId::Null,
                                    NodeContext* context        = nullptr,
                                    const NodeId& requestNodeId = NodeId::Null,  // usually want auto generated ids
                                    bool mandatory              = true)
    {
        T a[size_array]{};
        T type{};
        Variant value(type);
        VariableAttributes var_attr;
        //
        var_attr.setDefault();
        var_attr.setDisplayName(n);
        var_attr.setDescription(n);
        var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        Variant variant_array;
        variant_array.setArrayCopy(&a, size_array, value.get().type);
        var_attr.setValue(variant_array);
        //
        QualifiedName qn(m_nameSpace, n.c_str());
        //
        NodeId newNode;
        newNode.notNull();
        //
        if (m_server.addVariableNode(requestNodeId,
                                     parent,
                                     NodeId::HasComponent,
                                     qn,
                                     NodeId::BaseDataVariableType,
                                     var_attr,
                                     newNode,
                                     context)) {
            if (mandatory) {
                return m_server.addReference(newNode,
                                             NodeId::HasModellingRule,
                                             ExpandedNodeId::ModellingRuleMandatory,
                                             true);
            }
            if (!nodeId.isNull())
                nodeId = newNode;
            return true;
        }
        UAPRINTLASTERROR(m_server.lastError())
        return false;
    }

    bool addObjectTypeFolder(const std::string& childName,
                             const NodeId& parent,
                             NodeId& nodeId,
                             NodeId& requestNodeId = NodeId::Null,
                             bool mandatory        = true);
    
    /**
    * Add a Historical Variable node to a parent object type node.
    * @param T specify the UA_ built-in type.
    * @param name of the new Type node
    * @param parent of the new node.
    * @param context customize how the node will be created if not null.
    * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
    * @param mandatory specify if the node is mandatory in instances.
    * @return node id of the appended type on success, NodeId::Null otherwise.
     */

template <typename T>
    bool addHistoricalObjectTypeVariable(const std::string& n,
                                         const NodeId& parent,
                                         NodeId& nodeId        = NodeId::Null,
                                         NodeContext* context  = nullptr,
                                         NodeId& requestNodeId = NodeId::Null,  // usually want auto generated ids
                                         bool mandatory        = true)
    {
        T a{};
        Variant value(a);
        //
        VariableAttributes var_attr;
        var_attr.setDefault();
        var_attr.setDisplayName(n);
        var_attr.setDescription(n);
        var_attr.get().accessLevel =
            UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYREAD;
        var_attr.setValue(value);
        var_attr.get().dataType    = value.get().type->typeId;
        var_attr.get().historizing = true;

        //
        QualifiedName qn(m_nameSpace, n.c_str());
        //
        NodeId newNode;
        newNode.notNull();
        //
        if (m_server.addVariableNode(requestNodeId,
                                     parent,
                                     NodeId::HasComponent,
                                     qn,
                                     NodeId::BaseDataVariableType,
                                     var_attr,
                                     newNode,
                                     context)) {
            if (mandatory) {
                return m_server.addReference(newNode,
                                             NodeId::HasModellingRule,
                                             ExpandedNodeId::ModellingRuleMandatory,
                                             true);
            }
            if (!nodeId.isNull()) {
                nodeId = newNode;
            }
            return true;
        }
        UAPRINTLASTERROR(m_server.lastError())
        return false;
    }

    /*!
        \brief setMandatory
        \param n1
        \return
    */
    bool setMandatory(const NodeId& n1);

    /*!
        \brief addDerivedObjectType
        \param server
        \param n
        \param parent
        \param typeId
        \return
    */
    NodeId addDerivedObjectType(const std::string& n,
                              const NodeId& parent,
                              NodeId& typeId,
                              const NodeId& requestNodeId = NodeId::Null,
                              NodeContext* context        = nullptr);
    /*!
        \brief addChildren
        \return
    */
    virtual bool addChildren(const NodeId& /*parent*/) { return true; }

        /**
     * Add an instance of this object type.
     * @param name of the instance.
     * @param parent of the instance base node.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @return node id of the appended type on success, NodeId::Null otherwise.
     */
    virtual NodeId addInstance(const std::string& name,
                             const NodeId& parent,
                             NodeId& nodeId,
                             const NodeId& requestNodeId = NodeId::Null,
                             NodeContext* context        = nullptr);

    /**
     * Append a node to a parent as a derived object type.
     * The derived object type's children are added as well.
     * @param parent of the appended node object type
     * @param requestNodeId
     * @return node id of the appended type on success, NodeId::Null otherwise.
     */
    virtual NodeId append(const NodeId& parent,
                          NodeId& nodeId,
                          const NodeId& requestNodeId = NodeId::Null);  // derived type

    /**
     * Add the object type and its children.
     * @param nodeId specify the id of the base node of the type
     * @return true on success, false otherwise
     */
    virtual bool addType(const NodeId& nodeId);

};

} // namespace Open62541

#endif /* SERVEROBJECTTYPE_H */
