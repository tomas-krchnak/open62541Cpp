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
#include "serverobjecttype.h"

namespace Open62541 {

bool ServerObjectType::addBaseObjectType(
    const std::string&  name,
    NodeId&             requestNodeId,
    NodeContext*        context)
{
    ObjectTypeAttributes dtAttr;
    dtAttr.setDisplayName(name);
    _typeId.notNull();

    return _server.addObjectTypeNode(
        requestNodeId,
        NodeId::BaseObjectType,
        NodeId::HasSubType,
        QualifiedName(_nameSpace, name),
        dtAttr,
        _typeId,
        context);
}

//*****************************************************************************

bool ServerObjectType::addObjectTypeFolder(
    const std::string&  name,
    NodeId&             parent,
    NodeId&             nodeId,
    NodeId&             requestNodeId   /*= NodeId::Null*/,
    bool                mandatory       /*= true*/)
{
    NodeId newNode;
    newNode.notNull();

    if (!_server.addFolder(parent, name, newNode, requestNodeId))
        return false;

    if (mandatory)
        return setMandatory(newNode);

    if (!nodeId.isNull())
        nodeId = newNode;

    return true;
}

//*****************************************************************************

bool ServerObjectType::setMandatory(NodeId& node) {
    return _server.markMandatory(node);
}

//*****************************************************************************

bool ServerObjectType::addDerivedObjectType(
    const std::string&  name,
    NodeId&             parent,
    NodeId&             nodeId          /*= NodeId::Null*/,
    NodeId&             requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/)
{
    ObjectTypeAttributes attr;
    attr.setDisplayName(name);
    
    return _server.addObjectTypeNode(
        requestNodeId,
        parent,
        NodeId::HasSubType,
        QualifiedName(_nameSpace, name),
        attr,
        nodeId,
        context);
}

//*****************************************************************************

bool ServerObjectType::addType(NodeId& nodeId)
{ 
    if (addBaseObjectType(_name, nodeId))
        return addChildren(_typeId);

    return false;
}

//*****************************************************************************

bool ServerObjectType::append(
    NodeId& parent,
    NodeId& nodeId,
    NodeId& requestNodeId) // derived type - returns node id of the appended type
{
    if (addDerivedObjectType(_name, parent, nodeId, requestNodeId))
        return addChildren(nodeId);

    return false;
}

//*****************************************************************************

bool ServerObjectType::addInstance(
    const std::string&  name,
    NodeId&             parent,
    NodeId&             nodeId,
    NodeId&             requestNodeId,
    NodeContext*        context) {

    bool ret = _server.addInstance(
        name,
        requestNodeId,
        parent,
        _typeId,
        nodeId,
        context);

   UAPRINTLASTERROR(_server.lastError());
   return ret;
}

} // namespace Open62541
