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
    /**
     * NodeContext
     * @param s
     */
    NodeContext(const std::string &s = "") : _name(s) {

    }

    /**
     * ~NodeContext
     */
    virtual ~NodeContext() {}

    /**
     * name
     * @return 
     */
    const std::string & name() { return _name; }

    /**
     * find
     * @param s
     * @return 
     */

    // global life-cycle construct and destruct

    /**
     * construct
     * @param node
     */
    virtual bool construct(Server&, NodeId&) {
        return true; // doing nothing is OK
    }

    /**
     * destruct
     */
    virtual void destruct(Server&, NodeId&) {
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
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* typeNodeId, void* typeNodeContext,
        const UA_NodeId* nodeId, void** nodeContext);

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
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* typeNodeId, void* typeNodeContext,
        const UA_NodeId* nodeId, void** nodeContext);
    
    /**
     * typeConstruct
     * @return true on success
     */
    virtual bool typeConstruct(Server& server, NodeId& n, NodeId& t) {
        return true;
    }

    /**
     * typeDestruct
     * @param server
     * @param n
     */
    virtual void typeDestruct(Server& server, NodeId& n, NodeId& t) {
    }

    /**
     * setTypeLifeCycle
     * @param server
     * @param n
     * @return true on success
     */
    bool setTypeLifeCycle(Server &server, NodeId &n);

    // Set up the data and value callbacks

    /**
     * readData
     * @param node
     * @param range
     * @param value
     * @return true on success
     */
    virtual bool readData(Server& server,  NodeId& node, const UA_NumericRange* range, UA_DataValue& value) {
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
    virtual bool writeData(Server& server,  NodeId& node, const UA_NumericRange* range, const UA_DataValue& value) {
        return false;
    }

    /**
     * setAsDataSource
     * @param server
     * @param n
     * @return true on success
     */
    bool setAsDataSource(Server &server,  NodeId &n);

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
    static UA_StatusCode readDataSource(UA_Server* server, const UA_NodeId* sessionId,
                                        void* sessionContext, const UA_NodeId* nodeId,
                                        void* nodeContext, UA_Boolean includeSourceTimeStamp,
                                        const UA_NumericRange* range, UA_DataValue* value);

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
    static UA_StatusCode writeDataSource(UA_Server* server, const UA_NodeId* sessionId,
                                            void* sessionContext, const UA_NodeId* nodeId,
                                            void* nodeContext, const UA_NumericRange* range,
                                            const UA_DataValue* value);

    /**
     * setValueCallback
     * @param server
     * @param n
     * @return true on success
     */
    bool setValueCallback(Server &server, NodeId &n);

    /**
     * readValue
     * @param node
     */
    virtual void readValue(Server& server, NodeId& node, const UA_NumericRange* range, const UA_DataValue* value) {}

    /**
     * writeValue
     * @param node
     */
    virtual void writeValue(Server& server, NodeId& node, const UA_NumericRange* range, const UA_DataValue& value) {}

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
    static void readValueCallback(UA_Server* server, const UA_NodeId* sessionId,
                                    void* sessionContext, const UA_NodeId* nodeid,
                                    void* nodeContext, const UA_NumericRange* range,
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
    static void writeValueCallback(UA_Server* server, const UA_NodeId* sessionId,
                                    void* sessionContext, const UA_NodeId* nodeId,
                                    void* nodeContext, const UA_NumericRange* range,
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
     * @param n
     */
    RegisteredNodeContext(const std::string &n) : NodeContext(n) {
        _map[n] = this; // self register
    }

    /**
     * ~RegisteredNodeContext
     */
    virtual ~RegisteredNodeContext() {
        _map.erase(name()); // deregister on delete
    }

    /**
     * findRef
     * @param s
     * @return 
     */
    static NodeContext* findRef(const std::string &s) {
        return _map[s];
    }
};

} // namespace Open62541

#endif // METHODCONTEXT_H
