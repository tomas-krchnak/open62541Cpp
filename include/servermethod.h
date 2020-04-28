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

#ifndef SERVERMETHOD_H
#define SERVERMETHOD_H

#include "open62541objects.h"
#include "nodecontext.h"

namespace Open62541 {

/**
 * The ServerMethod class
 */
class UA_EXPORT ServerMethod : public NodeContext {
    const std::string   _name;
    ArgumentList        _in;
    ArgumentList        _out;

public:
    /**
     * methodCallback
     * @param handle
     * @param objectId
     * @param inputSize
     * @param input
     * @param outputSize
     * @param output
     * @return 
     */
    static UA_StatusCode methodCallback(
        UA_Server* server, const UA_NodeId* sessionId,
        void* sessionContext, const UA_NodeId* methodId,
        void* methodContext, const UA_NodeId* objectId,
        void* objectContext, size_t inputSize,
        const UA_Variant* input, size_t outputSize,
        UA_Variant* output);

protected:
    UA_StatusCode _lastError;

public:
    /**
     * ServerMethod
     * @param s
     * @param n
     * @param nInputs
     * @param nOutputs
     */
    ServerMethod(const std::string& n,
                    int nInputs = 1,
                    int nOutputs = 1);

    virtual ~ServerMethod() {}

    /**
     * in
     * @return 
     */
    ArgumentList& in() {
        return _in;
    }

    /**
     * out
     * @return 
     */
    ArgumentList& out() {
        return _out;
    }

    /**
     * callback
     * @return 
     */
    virtual UA_StatusCode callback(Server& /*server*/,
                                    const UA_NodeId* /*objectId*/,
                                    size_t /*inputSize*/,
                                    const UA_Variant* /*input*/,
                                    size_t /*outputSize*/,
                                    UA_Variant* /*output*/) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * lastOK
     * @return 
     */
    bool lastOK() {
        return _lastError == UA_STATUSCODE_GOOD;
    }

    /**
     * setMethodNodeCallBack
     * @param s
     * @param node
     * @return 
     */
    bool setMethodNodeCallBack(Server& s, NodeId& node);

    /**
     * addServerMethod
     * @param browseName
     * @param parent
     * @param nodeId
     * @param newNode
     * @param nameSpaceIndex
     * @return 
     */
    bool addServerMethod(
        Server& s,
        const std::string& browseName,
        NodeId& parent,
        NodeId& nodeId,
        NodeId& newNode = NodeId::Null,
        int nameSpaceIndex = 0);
};

/**
 * ServerMethodRef
 */
typedef std::shared_ptr<ServerMethod> ServerMethodRef;

} // namespace Open62541

#endif // SERVERMETHOD_H
