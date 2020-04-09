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

// prepared objects
UA_DataSource NodeContext::_dataSource =
{
    NodeContext::readDataSource,
    NodeContext::writeDataSource
};

UA_ValueCallback NodeContext::_valueCallback =
{
   NodeContext::readValueCallback,
   NodeContext::writeValueCallback
};

UA_NodeTypeLifecycle NodeContext::_nodeTypeLifeCycle =
{
    NodeContext::typeConstructor,
    NodeContext::typeDestructor
};

//
// Default Datavalue
//
static Variant defaultValue("Undefined");

UA_StatusCode NodeContext::typeConstructor(UA_Server *server,
                              const UA_NodeId * /*sessionId*/, void * /*sessionContext*/,
                              const UA_NodeId *typeNodeId, void * /*typeNodeContext*/,
                              const UA_NodeId *nodeId, void **nodeContext)
{
    UA_StatusCode ret = (UA_StatusCode)(-1);
    if(server && nodeId && typeNodeId)
    {
        NodeContext *p = (NodeContext *)(*nodeContext);
        if(p)
        {
            Server *s = Server::findServer(server);
            if(s)
            {
                NodeId n;
                n = *nodeId;
                NodeId t;
                t = *typeNodeId;

                if(p->typeConstruct(*s,n,t))
                    ret = UA_STATUSCODE_GOOD;
            }
        }
    }
    return ret;
}

 void NodeContext::typeDestructor(UA_Server *server,
                    const UA_NodeId * /*sessionId*/, void * /*sessionContext*/,
                    const UA_NodeId *typeNodeId, void * /*typeNodeContext*/,
                    const UA_NodeId *nodeId, void **nodeContext)
 {
    if(server && nodeId && typeNodeId)
    {
        NodeContext *p = (NodeContext *)(*nodeContext);
        if(p)
        {
            Server *s = Server::findServer(server);
            if(s)
            {
                NodeId n;
                n = *nodeId;
                NodeId t;
                t = *typeNodeId;

                p->typeDestruct(*s,n,t);
            }
        }
    }
 }


bool NodeContext::setTypeLifeCycle(Server &server,NodeId &n)
{
    return UA_Server_setNodeTypeLifecycle(server.server(), n,_nodeTypeLifeCycle) == UA_STATUSCODE_GOOD;
}

bool NodeContext::setAsDataSource(Server &server, NodeId &n)
{
    // Make this context handle the data source calls
    return UA_Server_setVariableNode_dataSource(server.server(), n,
                                         _dataSource) == UA_STATUSCODE_GOOD;
}

UA_StatusCode NodeContext::readDataSource(UA_Server *server, const UA_NodeId * /*sessionId*/,
                          void * /*sessionContext*/, const UA_NodeId *nodeId,
                          void *nodeContext, UA_Boolean includeSourceTimeStamp,
                          const UA_NumericRange *range, UA_DataValue *value)
{
    UA_StatusCode ret = UA_STATUSCODE_GOOD;
    if(nodeContext)
    {
        NodeContext *p = (NodeContext *)(nodeContext); // require node contexts to be NULL or NodeContext objects
        Server *s = Server::findServer(server);
        if(s && p && nodeId && value )
        {
            NodeId n;
            n = *nodeId;
            if(!p->readData(*s, n, range, *value))
            {
                ret = UA_STATUSCODE_BADDATAUNAVAILABLE;
            }
            else
            {
                if(includeSourceTimeStamp)
                {
                    value->hasServerTimestamp = true;
                    value->sourceTimestamp = UA_DateTime_now();
                }
            }
        }
    }
    return ret;
}

UA_StatusCode NodeContext::writeDataSource(UA_Server *server, const UA_NodeId * /*sessionId*/,
                          void * /*sessionContext*/, const UA_NodeId *nodeId,
                          void *nodeContext,
                          const UA_NumericRange *range, // can be null
                          const UA_DataValue *value)
{
    UA_StatusCode ret = UA_STATUSCODE_GOOD;
    if(nodeContext)
    {
        NodeContext *p = (NodeContext *)(nodeContext); // require node contexts to be NULL or NodeContext objects
        Server *s = Server::findServer(server);
        if(s && p && nodeId && value)
        {
            NodeId n;
            n = *nodeId;
            if(!p->writeData(*s, n, range, *value))
            {
                ret = UA_STATUSCODE_BADDATAUNAVAILABLE;
            }
        }
    }
    return ret;
}

bool NodeContext::setValueCallback(Server &server, NodeId &n)
{
    return UA_Server_setVariableNode_valueCallback(server.server(),n,_valueCallback) == UA_STATUSCODE_GOOD;
}

// Value Callbacks
void NodeContext::readValueCallback(UA_Server *server, const UA_NodeId * /*sessionId*/,
                void * /*sessionContext*/, const UA_NodeId *nodeId,
                void *nodeContext,
                const UA_NumericRange *range, // can be null
                const UA_DataValue *value)
{
    if(nodeContext)
    {
        NodeContext *p = (NodeContext *)(nodeContext); // require node contexts to be NULL or NodeContext objects
        Server *s = Server::findServer(server);
        if(s && p && nodeId && value )
        {
           NodeId n = *nodeId;
           p->readValue(*s, n, range, value);
        }
    }
}

void NodeContext::writeValueCallback(UA_Server *server,
                    const UA_NodeId * /*sessionId*/,
                    void * /*sessionContext*/,
                    const UA_NodeId *nodeId,
                    void *nodeContext,
                    const UA_NumericRange *range, // can be null
                    const UA_DataValue *value)
{
    if(nodeContext)
    {
        NodeContext *p = (NodeContext *)(nodeContext); // require node contexts to be NULL or NodeContext objects
        Server *s = Server::findServer(server);
        if(s && p && nodeId && value)
        {
            NodeId n = *nodeId;
            p->writeValue(*s, n, range, *value);
        }
    }
}

} // namespace Open62541
