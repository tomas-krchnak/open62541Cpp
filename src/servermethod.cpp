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
    UA_Server*          pUAServer,
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
    if (!methodContext)
        return UA_STATUSCODE_GOOD;

    if (auto pServer = Server::findServer(pUAServer))
    {
        VariantList inputs;
        inputs.assign(input, input + inputSize);

        return ((ServerMethod*)methodContext)->callback(
            *pServer,
            objectId,
            inputs,
            VariantSpan(output, outputSize));
    }

    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

ServerMethod::ServerMethod(
    const std::string&  node,
    int                 nInputs,
    int                 nOutputs)
    : NodeContext(node)
{
    m_in.resize(nInputs + 1); // create parameter space
    m_out.resize(nOutputs + 1);
}

ServerMethod::ServerMethod(const std::string &n,MethodFunc f,
                                      int nInputs,
                                      int nOutputs) : NodeContext(n), _func(f) {
    _in.resize(nInputs + 1); // create parameter space
    _out.resize(nOutputs + 1);
}


bool ServerMethod::setMethodNodeCallBack(Server& server, NodeId& node)
{
    return server.server()
        ? (UA_Server_setMethodNode_callback(
            server.server(),
            node,
            methodCallback) == UA_STATUSCODE_GOOD)
        : false;
}

//*****************************************************************************

bool ServerMethod::addServerMethod(
    Server&             server,
    const std::string&  browseName,
    const NodeId&       parent,
    const NodeId&       nodeId,
    NodeId&             newNode         /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/)
{
    // adding a method allocates in/out variable space
    return server.addMethod(
        this,
        browseName,
        parent,
        nodeId,
        newNode,
        nameSpaceIndex);
}

} // namespace Open62541
