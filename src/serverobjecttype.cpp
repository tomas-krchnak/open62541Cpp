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

ServerObjectType::ServerObjectType(Server &s, const std::string &n) : _server(s),  _name(n) {
}

ServerObjectType::~ServerObjectType() {

}

bool ServerObjectType::addBaseObjectType(const std::string &n,
                                                    NodeId &requestNodeId,
                                                    NodeContext *context) {
    ObjectTypeAttributes dtAttr;
    QualifiedName qn(_nameSpace, n);
    dtAttr.setDisplayName(n);
    _typeId.notNull();
    return _server.addObjectTypeNode(requestNodeId,
                                     NodeId::BaseObjectType,
                                     NodeId::HasSubType,
                                     qn,
                                     dtAttr,
                                     _typeId,context);
}

bool ServerObjectType::addDerivedObjectType(const std::string &n,
                                                       NodeId &parent,
                                                       NodeId &typeId,
                                                       NodeId &requestNodeId ,
                                                       NodeContext *context) {
    ObjectTypeAttributes ptAttr;
    ptAttr.setDisplayName(n);
    QualifiedName qn(_nameSpace, n);
    //
    return _server.addObjectTypeNode(requestNodeId, parent, NodeId::HasSubType, qn,
                                     ptAttr, typeId,context);
}

bool ServerObjectType::addType(NodeId &nodeId) { // base node of type
    if (addBaseObjectType(_name, nodeId)) {
        return addChildren(_typeId);
    }
    return false;
}

bool ServerObjectType::append(NodeId &parent, NodeId &nodeId, NodeId &requestNodeId) { // derived type - returns node id of append type
    if (addDerivedObjectType(_name, parent, nodeId, requestNodeId)) {
        return addChildren(nodeId);
    }
    return false;
}

bool ServerObjectType::addInstance(const std::string &n, NodeId &parent,
                                              NodeId &nodeId, NodeId &requestNodeId, NodeContext *context) {
   bool ret = _server.addInstance(n,
                               requestNodeId,
                               parent,
                               _typeId,
                               nodeId,
                               context);
   UAPRINTLASTERROR(_server.lastError());
   return ret;
}

} // namespace Open62541
