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
#include <open62541cpp/serverobjecttype.h>
#include <open62541cpp/objects/QualifiedName.h>
#include <open62541cpp/objects/ObjectTypeAttributes.h>
#include <open62541cpp/objects/ExpandedNodeId.h>
#include <open62541cpp/open62541server.h>

namespace Open62541 {

bool ServerObjectType::addBaseObjectType(
    const std::string&  name,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/)
{
    m_typeId.notNull();

    return m_server.addObjectTypeNode(
        requestNodeId,
        NodeId::BaseObjectType,
        NodeId::HasSubType,
        QualifiedName(m_nameSpace, name),
        ObjectTypeAttributes()
            .setDisplayName(name),
        m_typeId,
        context);
}

//*****************************************************************************

NodeId ServerObjectType::addDerivedObjectType(
    const std::string&  name,
    const NodeId&       parent,
    NodeId& typeId,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/)
{
    NodeId newNode;
    newNode.notNull();

    if (m_server.addObjectTypeNode(
            requestNodeId,
            parent,
            NodeId::HasSubType,
            QualifiedName(m_nameSpace, name),
            ObjectTypeAttributes()
                .setDisplayName(name),
            typeId,
            context))
        return typeId;

    return {};
}

//*****************************************************************************

bool ServerObjectType::addType(const NodeId& nodeId)
{
    if (addBaseObjectType(m_name, nodeId))
        return addChildren(m_typeId);

    return false;
}

//*****************************************************************************

NodeId ServerObjectType::append(
    const NodeId& parent,
    NodeId& nodeId,
    const NodeId& requestNodeId /*= NodeId::Null*/)
{
    if (auto newNode = addDerivedObjectType(m_name, parent, nodeId, requestNodeId))
        if (addChildren(requestNodeId))
            return newNode;

    return {};
}

//*****************************************************************************

NodeId ServerObjectType::addInstance(const std::string& name,
                                     const NodeId& parent,
                                     NodeId& nodeId,
                                     const NodeId& requestNodeId,
                                     NodeContext* context)
{
    bool ret = m_server.addInstance(
        name,
        requestNodeId,
        parent,
        m_typeId,
        nodeId,
        context);

   UAPRINTLASTERROR(m_server.lastError());
    return ret ? nodeId : NodeId::Null;
}

bool ServerObjectType::addObjectTypeFolder(const std::string& childName,
                         const NodeId& parent,
                         NodeId& nodeId,
                         NodeId& requestNodeId,
                         bool mandatory)
{
    NodeId newNode;
    newNode.notNull();

    if (m_server.addFolder(parent, childName, newNode, requestNodeId)) {
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
    return false;
}

bool ServerObjectType::setMandatory(const NodeId& n1)
{
    return m_server.addReference(n1, NodeId::HasModellingRule, ExpandedNodeId::ModellingRuleMandatory, true) ==
           UA_STATUSCODE_GOOD;
}

/*!
    \brief addObjectTypeVariable
    \param n
    \param parent
    \param nodeiD
    \param mandatory
    \return
*/
template <typename T>
bool ServerObjectType::addObjectTypeVariable(
    const std::string& n,
                           const NodeId& parent,
                           NodeId& nodeId,
                           NodeContext* context,
                           const NodeId& requestNodeId ,  // usually want auto generated ids
                           bool mandatory)
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

template <typename T>
bool ServerObjectType::addHistoricalObjectTypeVariable(
    const std::string& n,
                                     const NodeId& parent,
                                     NodeId& nodeId,        
                                     NodeContext* context,
                                     NodeId& requestNodeId,  // usually want auto generated ids
                                     bool mandatory)
{
    T a{};
    Variant value(a);
    //
    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(n);
    var_attr.setDescription(n);
    var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYREAD;
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

template <typename T, size_t size_array>
bool ServerObjectType::addObjectTypeArrayVariable(
    const std::string& n,
                                const NodeId& parent,
                                NodeId& nodeId,
                                NodeContext* context,
                                const NodeId& requestNodeId,  // usually want auto generated ids
                                bool mandatory)
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
} // namespace Open62541
