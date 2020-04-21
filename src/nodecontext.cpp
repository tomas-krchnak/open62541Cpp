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
#include "../include/nodecontext.h"
#include "../include/open62541server.h"

namespace Open62541 {

// set of contexts
RegisteredNodeContext::NodeContextMap RegisteredNodeContext::_map;

//*****************************************************************************
// prepared objects
UA_DataSource NodeContext::_dataSource =
{
    NodeContext::readDataSource,
    NodeContext::writeDataSource
};

//*****************************************************************************

UA_ValueCallback NodeContext::_valueCallback =
{
   NodeContext::readValueCallback,
   NodeContext::writeValueCallback
};

//*****************************************************************************

UA_NodeTypeLifecycle NodeContext::_nodeTypeLifeCycle =
{
    NodeContext::typeConstructor,
    NodeContext::typeDestructor
};

//*****************************************************************************
//*****************************************************************************

//
// Default Datavalue
//
static Variant defaultValue("Undefined");

UA_StatusCode NodeContext::typeConstructor(
    UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* typeNodeId, void* /*typeNodeContext*/,
    const UA_NodeId* nodeId, void** nodeContext)
{
    UA_StatusCode error = -1;

    if (!server || !nodeId || !typeNodeId)
        return error;
    
    auto pContext = (NodeContext*)(*nodeContext);
    auto pServer  = Server::findServer(server);
    if (!pContext || !pServer)
        return error;

    NodeId node = *nodeId;
    NodeId type = *typeNodeId;
    if(pContext->typeConstruct(*pServer, node, type))
        return UA_STATUSCODE_GOOD;

    return error;
}

//*****************************************************************************

 void NodeContext::typeDestructor(
     UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* typeNodeId, void* /*typeNodeContext*/,
    const UA_NodeId* nodeId, void** nodeContext)
 {
     if (!server || !nodeId || !typeNodeId)
         return;

    auto pContext = (NodeContext*)(*nodeContext);
    auto pServer  = Server::findServer(server);
    if (!pContext || !pServer)
        return;

    NodeId node = *nodeId;
    NodeId type = *typeNodeId;
    pContext->typeDestruct(*pServer, node, type);
}

//*****************************************************************************

bool NodeContext::setTypeLifeCycle(Server& server,NodeId& node)
{
    return UA_Server_setNodeTypeLifecycle(
        server.server(), node, _nodeTypeLifeCycle) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool NodeContext::setAsDataSource(Server& server, NodeId& node)
{
    // Make this context handle the data source calls
    return UA_Server_setVariableNode_dataSource(
        server.server(), node, _dataSource) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

UA_StatusCode NodeContext::readDataSource(
    UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* nodeId, void* nodeContext,
    UA_Boolean includeSourceTimeStamp,
    const UA_NumericRange* range, UA_DataValue* value)
{
    if (!nodeContext)
        return UA_STATUSCODE_GOOD;

    auto pContext = (NodeContext*)(nodeContext);
    auto pServer  = Server::findServer(server);
    if (!pServer || !pContext || !nodeId || !value)
        return UA_STATUSCODE_GOOD;

    NodeId node = *nodeId;
    if(!pContext->readData(*pServer, node, range, *value))
        return UA_STATUSCODE_BADDATAUNAVAILABLE;

    if(includeSourceTimeStamp)
    {
        value->hasServerTimestamp = true;
        value->sourceTimestamp = UA_DateTime_now();
    }
    
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

UA_StatusCode NodeContext::writeDataSource(
    UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, // can be null
    const UA_DataValue* value)
{
    if (!nodeContext)
        return UA_STATUSCODE_GOOD;

    auto pContext = (NodeContext*)(nodeContext);
    auto pServer  = Server::findServer(server);
    if (!pServer || !pContext || !nodeId || !value)
        return UA_STATUSCODE_GOOD;

    NodeId node = *nodeId;
    if(!pContext->writeData(*pServer, node, range, *value))
        return UA_STATUSCODE_BADDATAUNAVAILABLE;

    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool NodeContext::setValueCallback(Server& server, NodeId& node)
{
    return UA_Server_setVariableNode_valueCallback(
        server.server(), node, _valueCallback) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

// Value Callbacks
void NodeContext::readValueCallback(
    UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, // can be null
    const UA_DataValue* value)
{
    if(!nodeContext)
        return;

    auto pContext = (NodeContext*)(nodeContext);
    auto pServer  = Server::findServer(server);
    if(pServer && pContext && nodeId && value )
    {
       NodeId node = *nodeId;
       pContext->readValue(*pServer, node, range, value);
    }
}

//*****************************************************************************

void NodeContext::writeValueCallback(
    UA_Server* server,
    const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, // can be null
    const UA_DataValue* value)
{
    if(!nodeContext)
        return;

    auto pContext = (NodeContext*)(nodeContext);
    auto pServer  = Server::findServer(server);
    if(pServer && pContext && nodeId && value)
    {
        NodeId node = *nodeId;
        pContext->writeValue(*pServer, node, range, *value);
    }
}

} // namespace Open62541
