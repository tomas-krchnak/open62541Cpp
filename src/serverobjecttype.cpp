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
/*!
    \brief ~ServerObjectType
*/
Open62541::ServerObjectType::~ServerObjectType() {}

/*!
    \brief addBaseObjectType
    \param n
    \param typeId
    \return
*/
bool Open62541::ServerObjectType::addBaseObjectType(const std::string& n,
                                                    const NodeId& requestNodeId,
                                                    NodeContext* context)
{
    ObjectTypeAttributes dtAttr;
    QualifiedName qn(_nameSpace, n);
    dtAttr.setDisplayName(n);
    _typeId.notNull();
    return _server
        .addObjectTypeNode(requestNodeId, NodeId::BaseObjectType, NodeId::HasSubType, qn, dtAttr, _typeId, context);
}

//*****************************************************************************

NodeId ServerObjectType::addDerivedObjectType(
    const std::string&  name,
    const NodeId&       parent,
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
            newNode,
            context))
        return newNode;

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
    const NodeId& requestNodeId /*= NodeId::Null*/)
{
    if  (auto newNode = addDerivedObjectType(m_name, parent, requestNodeId))
        if (addChildren(requestNodeId))
            return newNode;

    return {};
}

//*****************************************************************************

NodeId ServerObjectType::addInstance(
    const std::string&  name,
    const NodeId&       parent,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/) {

    NodeId newNode;
    newNode.notNull();

    bool ret = m_server.addInstance(
        name,
        requestNodeId,
        parent,
        m_typeId,
        newNode,
        context);

   UAPRINTLASTERROR(m_server.lastError());
   return ret ? newNode : NodeId::Null;
}

} // namespace Open62541
