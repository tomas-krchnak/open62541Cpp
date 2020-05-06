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
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/)
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
    const NodeId&       parent,
    NodeId&             outNewNodeId    /*= NodeId::Null*/,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    bool                mandatory       /*= true*/)
{
    NodeId newNode;
    newNode.notNull();

    if (!_server.addFolder(parent, name, newNode, requestNodeId))
        return false;

    if (mandatory)
        return setMandatory(newNode);

    if (!outNewNodeId.isNull())
        outNewNodeId = newNode;

    return true;
}

//*****************************************************************************

bool ServerObjectType::setMandatory(const NodeId& node) {
    return _server.markMandatory(node);
}

//*****************************************************************************

bool ServerObjectType::addDerivedObjectType(
    const std::string&  name,
    const NodeId&       parent,
    NodeId&             outNewNodeId    /*= NodeId::Null*/,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
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
        outNewNodeId,
        context);
}

//*****************************************************************************

bool ServerObjectType::addType(const NodeId& nodeId)
{ 
    if (addBaseObjectType(_name, nodeId))
        return addChildren(_typeId);

    return false;
}

//*****************************************************************************

bool ServerObjectType::append(
    const NodeId& parent,
    NodeId&       outNewNodeId  /*= NodeId::Null*/, // derived type - returns node id of the appended type
    const NodeId& requestNodeId /*= NodeId::Null*/) 
{
    if (addDerivedObjectType(_name, parent, outNewNodeId, requestNodeId))
        return addChildren(requestNodeId);

    return false;
}

//*****************************************************************************

bool ServerObjectType::addInstance(
    const std::string&  name,
    const NodeId&       parent,
    NodeId&             outNewNodeId    /*= NodeId::Null*/,
    const NodeId&       requestNodeId   /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/) {

    bool ret = _server.addInstance(
        name,
        requestNodeId,
        parent,
        _typeId,
        outNewNodeId,
        context);

   UAPRINTLASTERROR(_server.lastError());
   return ret;
}

} // namespace Open62541
