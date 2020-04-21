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
#include <servermethod.h>
#include <open62541server.h>

namespace Open62541 {

UA_StatusCode ServerMethod::methodCallback(
    UA_Server*          server,
    const UA_NodeId*  /*sessionId*/,
    void*             /*sessionContext*/,
    const UA_NodeId*  /*methodId*/,
    void*               methodContext,
    const UA_NodeId*    objectId,
    void*             /*objectContext*/,
    size_t              inputSize,
    const UA_Variant*   input,
    size_t              outputSize,
    UA_Variant*         output)
{
    UA_StatusCode ret = UA_STATUSCODE_GOOD;
    if (methodContext) {
        Server  * s = Server::findServer(server);
        if(s)
        {
            ServerMethod *p = (ServerMethod *)methodContext;
            ret = p->callback(*s, objectId, inputSize, input, outputSize, output); // adding a method allocates in/out variable space
        }
    }
    return ret;
}

ServerMethod::ServerMethod(
    const std::string&  n,
    int                 nInputs,
    int                 nOutputs)
    : NodeContext(n)
{
    _in.resize(nInputs + 1); // create parameter space
    _out.resize(nOutputs + 1);
}

bool ServerMethod::setMethodNodeCallBack(Server &s, NodeId &node)
{
    return s.server()? (UA_Server_setMethodNode_callback(s.server(), node, methodCallback) == UA_STATUSCODE_GOOD):false;
}

bool ServerMethod::addServerMethod(
    Server& s,
    const std::string& browseName,
    NodeId& parent,
    NodeId& nodeId,
    NodeId& newNode     /*= NodeId::Null*/,
    int nameSpaceIndex  /*= 0*/)
{
    return s.addServerMethod(this,browseName,parent,nodeId,newNode,nameSpaceIndex);
}

} // namespace Open62541
