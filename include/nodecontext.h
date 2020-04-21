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

#ifndef NODECONTEXT_H
#define NODECONTEXT_H

#include "open62541objects.h"

namespace Open62541 {

/**
 * The NodeContext class
 * Node context objects operate on nodes - the node contexts may be shared between more than one node
 * As of version 0.3 the node context is passed to callbacks and so can be used
 * This aggregates the data value call backs and value call backs and lifecycle callbacks
 */
class UA_EXPORT NodeContext {
    std::string _name;                              /**< Context name */
    static UA_DataSource _dataSource;               /**< Call back for data source operations */
    static UA_ValueCallback _valueCallback;         /**< call back for value get / set */
    static UA_NodeTypeLifecycle _nodeTypeLifeCycle; /**< life cycle callback */

public:
    NodeContext(const std::string& name = "") : _name(name)   {}
    virtual ~NodeContext()                              {}

    const std::string& name() { return _name; }

    // global life-cycle construct and destruct

    /**
     * construct a node in a given server
     * @param server where the new node will be constructed.
     * @param node can specify the data for the new node in input and/or the new node in output.
     */
    virtual bool construct(Server& server, NodeId& node) {
        return true; // doing nothing is OK
    }

    /**
     * destruct
     * @param server where the new node will be destroyed.
     * @param node specify the id of the node to destroy. Can also return the destroyed node.
     */
    virtual void destruct(Server& server, NodeId& node) {
    }

    // type life-cycle

    /**
     * typeConstructor
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param typeNodeId
     * @param typeNodeContext
     * @param nodeId
     * @param nodeContext
     * @return error code
     */
    static UA_StatusCode typeConstructor(
        UA_Server* server,
        const UA_NodeId* sessionId,  void*  sessionContext,
        const UA_NodeId* typeNodeId, void*  typeNodeContext,
        const UA_NodeId* nodeId,     void** nodeContext);

    /**
     * typeDestructor
     * Can be NULL. May replace the nodeContext.
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param typeNodeId
     * @param typeNodeContext
     * @param nodeId
     * @param nodeContext
     */
    static void typeDestructor(
        UA_Server* server,
        const UA_NodeId* sessionId,  void*  sessionContext,
        const UA_NodeId* typeNodeId, void*  typeNodeContext,
        const UA_NodeId* nodeId,     void** nodeContext);
    
    /**
     * typeConstruct
     * @param server
     * @param node
     * @param type
     * @return true on success
     */
    virtual bool typeConstruct(Server& server, NodeId& node, NodeId& type) {
        return true;
    }

    /**
     * typeDestruct
     * @param server
     * @param node
     * @param type
     */
    virtual void typeDestruct(Server& server, NodeId& node, NodeId& type) {
    }

    /**
     * setTypeLifeCycle
     * @param server
     * @param node
     * @return true on success
     */
    bool setTypeLifeCycle(Server& server, NodeId& node);

    // Set up the data and value callbacks

    /**
     * readData
     * @param node
     * @param range
     * @param value
     * @return true on success
     */
    virtual bool readData(
        Server& server,
        NodeId& node,
        const UA_NumericRange* range,
        UA_DataValue& value) {
        return false;
    }

    /**
     * writeData
     * @param server
     * @param node
     * @param range
     * @param value
     * @return true on success
     */
    virtual bool writeData(
        Server& server,
        NodeId& node,
        const UA_NumericRange* range,
        const UA_DataValue& value) {
        return false;
    }

    /**
     * setAsDataSource
     * @param server
     * @param node
     * @return true on success
     */
    bool setAsDataSource(Server& server,  NodeId& node);

    /**
     * readDataSource
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param nodeContext
     * @param includeSourceTimeStamp
     * @param range
     * @param value
     * @return error code
     */
    static UA_StatusCode readDataSource(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext,
        UA_Boolean includeSourceTimeStamp,
        const UA_NumericRange* range,
        UA_DataValue* value);

    /**
     * writeDataSource
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param nodeContext
     * @param range
     * @param value
     * @return error code
     */
    static UA_StatusCode writeDataSource(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext,
        const UA_NumericRange* range,
        const UA_DataValue* value);

    /**
     * setValueCallback
     * @param server
     * @param node
     * @return true on success
     */
    bool setValueCallback(Server& server, NodeId& node);

    /**
     * readValue
     * @param node
     */
    virtual void readValue(
        Server& server,
        NodeId& node,
        const UA_NumericRange* range,
        const UA_DataValue* value) {}

    /**
     * writeValue
     * @param node
     */
    virtual void writeValue(
        Server& server,
        NodeId& node,
        const UA_NumericRange* range,
        const UA_DataValue& value) {}

    // Value Callbacks

    /**
     * readValueCallback
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param nodeid
     * @param nodeContext
     * @param range
     * @param value
     */
    static void readValueCallback(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeid,    void* nodeContext,
        const UA_NumericRange* range,
        const UA_DataValue* value);

    /**
     * writeValueCallback
     * @param server
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param nodeContext
     * @param range
     * @param data
     */
    static void writeValueCallback(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext,
        const UA_NumericRange* range,
        const UA_DataValue* data);
};

/**
* The RegisteredNodeContext class
* Can be used to setup stock call backs
*/
class RegisteredNodeContext : public NodeContext
{
    typedef std::map<std::string, NodeContext*> NodeContextMap;    /**< map of contexts */
    static NodeContextMap _map; /**< map of registered contexts - typically a static instance is used to self register */

public:
    /**
     * RegisteredNodeContext
     * @param name of the registered context
     */
    RegisteredNodeContext(const std::string& name) : NodeContext(name) {
        _map[name] = this; // self register
    }

    /**
     * Dtor unregister on delete
     */
    virtual ~RegisteredNodeContext() {
        _map.erase(name());
    }

    /**
     * findRef
     * @param name
     * @return 
     */
    static NodeContext* findRef(const std::string& name) {
        return _map[name];
    }
};

} // namespace Open62541

#endif // METHODCONTEXT_H
