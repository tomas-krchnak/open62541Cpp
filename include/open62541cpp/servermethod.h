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
#include <open62541cpp/open62541objects.h>
#include <open62541cpp/objects/open62541typedefs.h>
#include <open62541cpp/nodecontext.h>

#ifndef BOOST_BEAST_CORE_SPAN_HPP
#include <boost/beast/core/span.hpp>
#endif
#include <open62541cpp/objects/ArgumentList.h>

namespace Open62541 {

using VariantSpan = boost::beast::span<UA_Variant>;
/**
 * The ServerMethod class
 */
typedef std::function<UA_StatusCode(Server&, const UA_NodeId*, size_t, const UA_Variant*, size_t, UA_Variant*)>
    MethodFunc;

class ServerMethod : public NodeContext {
    const std::string   m_name; /**< Name of the method */
    ArgumentList        m_in;   /**< List of input arguments for the method. */
    ArgumentList        m_out;  /**< List of output arguments of the method. */

protected:
    UA_StatusCode       m_lastError;
    MethodFunc _func;  // lambda
public:

    /**
     * Call-back used to call this method.
     * Customized by callback() hook.
     * @param server of the method node
     * @param sessionId     (unused)
     * @param sessionContext (unused)
     * @param methodId      (unused)
     * @param methodContext a pointer on this ServerMethod
     * @param objectId node of the method
     * @param objectContext (unused)
     * @param inputSize size of the input array
     * @param input data of the input array
     * @param outputSize size of the output array
     * @param output data of the output array
     * @return UA_STATUSCODE_GOOD
     */
    static UA_StatusCode methodCallback(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* methodId,  void* methodContext,
        const UA_NodeId* objectId,  void* objectContext,
        size_t inputSize, const UA_Variant* input,
        size_t outputSize, UA_Variant* output);

    /**
     * ServerMethod
     * @param name
     * @param nInputs
     * @param nOutputs
     */
    ServerMethod(
        const std::string&  name,
        int                 nInputs  = 1,
        int                 nOutputs = 1);
    /*!
     * \brief ServerMethod
     * \param n
     * \param f
     * \param nInputs
     * \param nOutputs
     */
    ServerMethod(
        const std::string& n, 
        MethodFunc f, 
        int nInputs = 0, 
        int nOutputs = 0);

    virtual ~ServerMethod() = default;

    /*!
     * \brief setFunction
     * \param f
     */
    void setFunction(MethodFunc f) { _func = f; }
    ArgumentList& in()      { return m_in; }
    ArgumentList& out()     { return m_out; }

    /**
     * Hook to customize methodCallback.
     * Do nothing by default.
     * @param server of the method node
     * @param objectId node of the method
     * @param inputSize size of the input array
     * @param input data of the input array
     * @param outputSize size of the output array
     * @param output data of the output array
     * @return UA_STATUSCODE_GOOD
     */
    virtual UA_StatusCode callback(
        Server&             server,
        const UA_NodeId*    objectId,
        const VariantList&  inputs,
              VariantSpan&  outputs) {
        m_lastError = UA_STATUSCODE_GOOD;
        return m_lastError;
    }

    /**
     * @return true if _lastError is UA_STATUSCODE_GOOD
     */
    bool lastOK() const { return m_lastError == UA_STATUSCODE_GOOD; }

    /**
     * Attach this method to an existing method node.
     * @param server of the node
     * @param node id of the method node.
     * @return true on success.
     */
    bool setMethodNodeCallBack(Server& server, NodeId& node);

    /**
     * Add a new method node to the server, thread-safely.
     * @param server of the new method node
     * @param browseName name of the method node
     * @param parent of the method node.
     * @param nodeId assigned node id or NodeId::Null for auto assign.
     * @param[out] newNode receives new node if not null.
     * @param nameSpaceIndex of new node. If 0, the parent namespace is used.
     * @return true on success.
     */
    bool addServerMethod(
        Server&             server,
        const std::string&  browseName,
        const NodeId&       parent,
        const NodeId&       nodeId,
        NodeId&             newNode         = NodeId::Null,
        int                 nameSpaceIndex  = 0);
};

/**
 * ServerMethodRef
 */
typedef std::shared_ptr<ServerMethod> ServerMethodRef;

} // namespace Open62541

#endif /* SERVERMETHOD_H */
