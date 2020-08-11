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
#include "open62541client.h"
#include "clientbrowser.h"

namespace Open62541 {

ApplicationDescriptionList::~ApplicationDescriptionList() {
    for (auto i : *this) {
        if (i) {
            UA_ApplicationDescription_delete(i); // delete the item
        }
    }
}

//*****************************************************************************
//*****************************************************************************

Client::~Client() {
    if (m_pClient) {
        disconnect();
        UA_Client_delete(m_pClient);
    }
}

//*****************************************************************************

bool Client::runIterate(uint32_t interval /*= 100*/) {
    if (!m_pClient) return false;

    m_lastError = UA_Client_run_iterate(m_pClient, interval);
    return lastOK();
}

//*****************************************************************************

void Client::initialise() {
    if (m_pClient) {
        if (getState() != UA_CLIENTSTATE_DISCONNECTED) disconnect();
        UA_Client_delete(m_pClient);
        m_pClient = nullptr;
    }
    m_pClient = UA_Client_new();
    if (!m_pClient)
        return;

    UA_ClientConfig_setDefault(UA_Client_getConfig(m_pClient)); // initalise the client structure
    UA_Client_getConfig(m_pClient)->clientContext = this;
    UA_Client_getConfig(m_pClient)->stateCallback = stateCallback;
    UA_Client_getConfig(m_pClient)->subscriptionInactivityCallback = subscriptionInactivityCallback;
}

//*****************************************************************************

void  Client::stateCallback (UA_Client* client, UA_ClientState clientState)
{
    if(auto p = (Client*)UA_Client_getContext(client)) {
        p->stateChange(clientState);
    }
}

//*****************************************************************************

void Client::asyncConnectCallback(
    UA_Client*  client,
    void*       userdata,
    UA_UInt32   requestId,
    void*       response) {
    if (auto p = (Client*)UA_Client_getContext(client)) {
        p->asyncConnectService(requestId, userdata, response);
    }
}

//*****************************************************************************

void Client::subscriptionInactivityCallback(
    UA_Client*  client,
    UA_UInt32   subscriptionId,
    void*       subContext) {
    if(auto p = (Client*)UA_Client_getContext(client)) {
        p->subscriptionInactivity(subscriptionId, subContext);
    }
}

//*****************************************************************************

bool Client::addSubscription(
    UA_UInt32&                  newId,
    CreateSubscriptionRequest*  settings /*= nullptr*/) {
    ClientSubscriptionRef sub(new ClientSubscription(*this));

    if (settings) {
        sub->settings() = *settings; // assign settings across
    }

    if (sub->create()) {
        newId = sub->id();
        m_subscriptions[newId] = sub;
        return true;
    }

    return false;
}

//*****************************************************************************

bool Client::removeSubscription(UA_UInt32 Id) {
    m_subscriptions.erase(Id); // remove from dictionary implicit delete
    return true;
}

//*****************************************************************************

ClientSubscription* Client::subscription(UA_UInt32 Id) {
    if (m_subscriptions.find(Id) != m_subscriptions.end()) {
        ClientSubscriptionRef& c = m_subscriptions[Id];
        return c.get();
    }
    return nullptr;
}

//*****************************************************************************

void Client::stateChange(UA_ClientState clientState) {
    switch (clientState) {
    case UA_CLIENTSTATE_DISCONNECTED:           stateDisconnected();        break;
    case UA_CLIENTSTATE_CONNECTED:              stateConnected();           break;
    case UA_CLIENTSTATE_SECURECHANNEL:          stateSecureChannel();       break;
    case UA_CLIENTSTATE_SESSION:                stateSession();             break;
    case UA_CLIENTSTATE_SESSION_RENEWED:        stateSessionRenewed();      break;
    case UA_CLIENTSTATE_WAITING_FOR_ACK:        stateWaitingForAck();       break;
    case UA_CLIENTSTATE_SESSION_DISCONNECTED:   stateSessionDisconnected(); break;
    default:                                                                break;
    }
}

//*****************************************************************************

bool Client::getEndpoints(
    const std::string&          serverUrl,
    EndpointDescriptionArray&   list) {
    if (!m_pClient) return false;

    UA_EndpointDescription* endpointDescriptions     = nullptr;
    size_t                  endpointDescriptionsSize = 0;
    {
        WriteLock l(m_mutex);
        m_lastError = UA_Client_getEndpoints(
            m_pClient, serverUrl.c_str(),
            &endpointDescriptionsSize,
            &endpointDescriptions);
    }
    if (!lastOK()) return false;

    // copy list so it is managed by the caller
    list.setList(endpointDescriptionsSize, endpointDescriptions);
    return true;
}

//*****************************************************************************

UA_StatusCode Client::getEndpoints(
    const std::string&        serverUrl,
    std::vector<std::string>& list) {
    if (!m_pClient) {
        throw std::runtime_error("Null client");
        return 0;
    }

    EndpointDescriptionArray endpoints;
    if (!getEndpoints(serverUrl, endpoints))
        return m_lastError;

    for (const auto& descr : endpoints)
        list.push_back(toString(descr.endpointUrl));

    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Client::findServers(
    const std::string&           serverUrl,
    const StringArray&           serverUris,
    const StringArray&           localeIds,
    ApplicationDescriptionArray& registeredServers) {
    if (!m_pClient) return false;

    WriteLock l(m_mutex);
    m_lastError = UA_Client_findServers(
        m_pClient,
        serverUrl.c_str(),
        serverUris.size(),
        serverUris.data(),
        localeIds.size(),
        localeIds.data(),
        registeredServers.sizeRef(),
        registeredServers.dataRef());
    UAPRINTLASTERROR(m_lastError)
        return lastOK();
}

//*****************************************************************************

bool Client::findServersOnNetwork(
    const std::string&      serverUrl,
    unsigned                startingRecordId,
    unsigned                maxRecordsToReturn,
    const StringArray&      serverCapabilityFilter,
    ServerOnNetworkArray&   serverOnNetwork) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_findServersOnNetwork(
        m_pClient, serverUrl.c_str(),
        startingRecordId,
        maxRecordsToReturn,
        serverCapabilityFilter.size(),
        serverCapabilityFilter.data(),
        serverOnNetwork.sizeRef(),
        serverOnNetwork.dataRef());
    return lastOK();
}

//*****************************************************************************

bool Client::readAttribute(
    const UA_NodeId&    nodeId,
    UA_AttributeId      attr,
    void*               outVal,
    const UA_DataType&  type) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = __UA_Client_readAttribute(m_pClient, &nodeId, attr, outVal, &type);
    return lastOK();
}

//*****************************************************************************

bool Client::writeAttribute(
    const UA_NodeId&    nodeId,
    UA_AttributeId      attr,
    const void*         val,
    const UA_DataType&  type) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = __UA_Client_writeAttribute(m_pClient, &nodeId, attr, val, &type);
    return lastOK();
}

//*****************************************************************************

UA_ClientState Client::getState() {
    ReadLock l(m_mutex);
    if (m_pClient) return UA_Client_getState(m_pClient);
    throw std::runtime_error("Null client");
    return UA_CLIENTSTATE_DISCONNECTED;
}

//*****************************************************************************

void Client::reset() {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    UA_Client_reset(m_pClient);
    return;
}

//*****************************************************************************

bool Client::connect(const std::string& endpointUrl) {
    initialise();
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_connect(m_pClient, endpointUrl.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::connectUsername(
    const std::string& endpoint,
    const std::string& username,
    const std::string& password) {
    initialise();
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_connect_username(
        m_pClient,
        endpoint.c_str(),
        username.c_str(),
        password.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::connectAsync(const std::string& endpoint) {
    initialise();
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_connect_async(
        m_pClient,
        endpoint.c_str(),
        asyncConnectCallback,
        this);
    return lastOK();
}

//*****************************************************************************

bool Client::connectNoSession(const std::string& endpoint) {
    initialise();
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_connect_noSession(m_pClient, endpoint.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::disconnect() {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_disconnect(m_pClient);
    return lastOK();
}

//*****************************************************************************

bool Client::disconnectAsync(UA_UInt32 requestId /*= 0*/) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_disconnect_async(m_pClient, &requestId);
    return lastOK();
}

//*****************************************************************************

int Client::namespaceGetIndex(const std::string& namespaceUri) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    int namespaceIndex = 0;
    UA_String uri = toUA_String(namespaceUri);
    if (UA_Client_NamespaceGetIndex(
        m_pClient,
        &uri,
        (UA_UInt16*)(&namespaceIndex)) == UA_STATUSCODE_GOOD) {
        return namespaceIndex;
    }
    return -1; // value
}

/******************************************************************************
* Call-back used to retrieve the list of children of a given node
* @param childId
* @param isInverse
* @param referenceTypeId
* @param handle
* @return UA_STATUSCODE_GOOD
*/
static UA_StatusCode browseTreeCallBack(
    UA_NodeId   childId,
    UA_Boolean  isInverse,
    UA_NodeId /*referenceTypeId*/,
    void*       outList) {
    if (!isInverse) { // not a parent node - only browse forward
        ((UANodeIdList*)outList)->put(childId);
    }
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

UANodeIdList Client::getChildrenList(const UA_NodeId& node) {
    UANodeIdList children;
    WriteLock ll(m_mutex);

    UA_Client_forEachChildNodeCall(
        m_pClient, node,
        browseTreeCallBack, // browse the tree
        &children);         // output arg of the call-back. hold the list

    return children; // NRVO
}

//*****************************************************************************

bool Client::browseTree(const UA_NodeId& nodeId, UANode* node) {
    if (!m_pClient) return false;
    
    for (auto& child : getChildrenList(nodeId)) {
        if (child.namespaceIndex < 1) continue;

        QualifiedName outBrowseName;
        if (!readBrowseName(child, outBrowseName)) continue;
        
        // create the node in the tree using the browse name as key
        NodeId dataCopy = child;        // deep copy
        UANode* pNewNode = node->createChild(toString(outBrowseName.name()));
        pNewNode->setData(dataCopy);
        browseTree(child, pNewNode);    // recurse
    }
    return lastOK();
}

//*****************************************************************************

bool Client::browseTree(const NodeId& nodeId, UANodeTree& outTree) {
    // form a hierarchical tree of nodes. given node is added to tree
    outTree.root().setData(nodeId); // set the root of the tree
    return browseTree(nodeId, outTree.rootNode());
}

//*****************************************************************************

bool Client::browseTree(const NodeId& nodeId, NodeIdMap& outNodeMap) {
    outNodeMap.put(nodeId);
    return browseChildren(nodeId, outNodeMap);
}

//*****************************************************************************

bool Client::browseChildren(const UA_NodeId& nodeId, NodeIdMap& nodeMap) {
    for (auto& child : getChildrenList(nodeId)) {
        if (child.namespaceIndex != nodeId.namespaceIndex)
            continue; // only in same namespace

        if (nodeMap.find(toString(child)) == nodeMap.end()) {
            nodeMap.put(child);
            browseChildren(child, nodeMap); // recurse no duplicates
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Client::nodeIdFromPath(const NodeId& start, const Path& path, NodeId& outNodeId) {
    // nodeId is a shallow copy - do not delete and is volatile
    UA_NodeId node = start.get();

    int level = 0;
    if (path.size() > 0) {
        ClientBrowser browser(*this);
        while (level < int(path.size())) {
            browser.browse(node);
            auto pNode = browser.find(path[level]);
            if (!pNode) return false;
            level++;
            node = pNode->nodeId;
        }
    }

    outNodeId = node; // deep copy
    return level == int(path.size());
}

//*****************************************************************************

bool Client::createFolderPath(
    const NodeId& start,
    const Path&   path,
    int           nameSpaceIndex,
    NodeId&       nodeId) {

    if (path.size() < 1)
        return true;

    UA_NodeId node = start.get();
    int level = 0;
    ClientBrowser browser(*this);

    while (level < int(path.size())) {
        browser.browse(node);
        auto pNode = browser.find(path[level]);
        if (!pNode) break;
        level++;
        node = pNode->nodeId; // shallow copy
    }
    if (level == int(path.size())) {
        nodeId = node;
    }
    else {
        NodeId nf(nameSpaceIndex, 0); // auto generate NODE id
        nodeId = node;
        NodeId newNode;
        while (level < int(path.size())) {
            addFolder(nodeId, path[level], nf, newNode.notNull(), nameSpaceIndex);
            if (!lastOK()) {
                break;
            }
            nodeId = newNode; // assign
            level++;
        }
    }
    
    return level == int(path.size());
}

//*****************************************************************************

bool Client::getChild(const NodeId& start, const std::string& childName, NodeId& ret) {
    Path path;
    path.push_back(childName);
    return nodeIdFromPath(start, path, ret);
}

//*****************************************************************************

bool Client::readBrowseName(const NodeId& nodeId, std::string& outName, int& outNamespace) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    QualifiedName outBrowseName;
    m_lastError = UA_Client_readBrowseNameAttribute(m_pClient, nodeId, outBrowseName);
    if (m_lastError == UA_STATUSCODE_GOOD) {
        outName = toString(outBrowseName->name);
        outNamespace = outBrowseName->namespaceIndex;
    }
    return m_lastError == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

void Client::setBrowseName(NodeId& nodeId, int nameSpaceIndex, const std::string& name) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    QualifiedName newBrowseName(nameSpaceIndex, name);
    UA_Client_writeBrowseNameAttribute(m_pClient, nodeId, newBrowseName);
}

//*****************************************************************************

bool Client::readArrayDimensions(
    const UA_NodeId&        nodeId,
    std::vector<UA_UInt32>& ret) {
    if (!m_pClient) return false;

    WriteLock l(m_mutex);
    size_t      outArrayDimensionsSize  = 0;
    UA_UInt32*  outArrayDimensions      = nullptr;
    m_lastError = UA_Client_readArrayDimensionsAttribute(
        m_pClient,
        nodeId,
        &outArrayDimensionsSize,
        &outArrayDimensions);

    if (m_lastError == UA_STATUSCODE_GOOD) {
        if (outArrayDimensions) {
            for (int i = 0; i < int(outArrayDimensionsSize); i++) {
                ret.push_back(outArrayDimensions[i]);
            }
            UA_Array_delete(
                outArrayDimensions,
                outArrayDimensionsSize,
                &UA_TYPES[UA_TYPES_INT32]);
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Client::setArrayDimensions(
    NodeId&                 nodeId,
    std::vector<UA_UInt32>& newArrayDimensions) {
    m_lastError = UA_Client_writeArrayDimensionsAttribute(
        m_pClient,
        nodeId,
        UA_UInt32(newArrayDimensions.size()),
        newArrayDimensions.data());
    return lastOK();
}

//*****************************************************************************

bool Client::deleteNode(const NodeId& nodeId, bool deleteReferences) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");
    m_lastError = UA_Client_deleteNode(m_pClient, nodeId, UA_Boolean(deleteReferences));
    return lastOK();
}

//*****************************************************************************

bool Client::deleteTree(const NodeId& nodeId) {
    if (!m_pClient) return false;

    NodeIdMap nodeMap;
    browseTree(nodeId, nodeMap);
    for (auto& node : nodeMap) {
        if (node.second.namespaceIndex > 0) { // namespace 0 appears to be reserved
            WriteLock l(m_mutex);
            UA_Client_deleteNode(m_pClient, node.second, true);
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Client::callMethod(
    const NodeId&       objectId,
    const NodeId&       methodId,
    const VariantList&  in,
    VariantArray&       out) {
    WriteLock l(m_mutex);
    if (!m_pClient) throw std::runtime_error("Null client");

    size_t      outputSize  = 0;
    UA_Variant* output      = nullptr;

    m_lastError = UA_Client_call(
        m_pClient,
        objectId,
        methodId,
        in.size(),
        in.data(),
        &outputSize,
        &output);

    if (!lastOK()) return false;

    out.setList(outputSize, output);
    return true;
}

//*****************************************************************************

bool Client::addFolder(
    const NodeId&       parent,
    const std::string&  browseName,
    const NodeId&       nodeId,
    NodeId&             outNewNodeId     /*= NodeId::Null*/,
    int                 nameSpaceIndex   /*= 0*/) {
    if(!m_pClient) return false;
    WriteLock l(m_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    ObjectAttributes attr;
    attr.setDisplayName(browseName);
    attr.setDescription(browseName);
    m_lastError = UA_Client_addObjectNode(
        m_pClient,
        nodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, browseName),
        NodeId::FolderType,
        attr.get(),
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addVariable(
    const NodeId&       parent,
    const std::string&  browseName,
    const Variant&      value,
    const NodeId&       nodeId,
    NodeId&             outNewNodeId     /*= NodeId::Null*/,
    int                 nameSpaceIndex   /*= 0*/) {
    if(!m_pClient) return false;
    WriteLock l(m_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    m_lastError = UA_Client_addVariableNode(
        m_pClient,
        nodeId, // Assign new/random NodeID
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, browseName),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        VariableAttributes()
            .setDisplayName(browseName)
            .setDescription(browseName)
            .setValue(value),
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addProperty(
    const NodeId&       parent,
    const std::string&  browseName,
    const Variant&      value,
    const NodeId&       nodeId,
    NodeId&             outNewNodeId    /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/) {
    if(!m_pClient) return false;
    WriteLock l(m_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    m_lastError = UA_Client_addVariableNode(
        m_pClient,
        nodeId, // Assign new/random NodeID
        parent,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        QualifiedName(nameSpaceIndex, browseName),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        VariableAttributes()
            .setDisplayName(browseName)
            .setDescription(browseName)
            .setValue(value),
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addVariableTypeNode(
    const NodeId&                 nodeId,
    const NodeId&                 parent,
    const NodeId&                 referenceTypeId,
    const QualifiedName&          browseName,
    const VariableTypeAttributes& attr,
    NodeId&                       outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addVariableTypeNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addObjectNode(
    const NodeId&             nodeId,
    const NodeId&             parent,
    const NodeId&             referenceTypeId,
    const QualifiedName&      browseName,
    const NodeId&             typeDefinition,
    const ObjectAttributes&   attr,
    NodeId&                   outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addObjectNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addObjectTypeNode(
    const NodeId&                 nodeId,
    const NodeId&                 parent,
    const NodeId&                 referenceTypeId,
    const QualifiedName&          browseName,
    const ObjectTypeAttributes&   attr,
    NodeId&                       outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addObjectTypeNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addViewNode(
    const NodeId&         nodeId,
    const NodeId&         parent,
    const NodeId&         referenceTypeId,
    const QualifiedName&  browseName,
    const ViewAttributes& attr,
    NodeId&               outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addViewNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addReferenceTypeNode(
    const NodeId&                  nodeId,
    const NodeId&                  parent,
    const NodeId&                  referenceTypeId,
    const QualifiedName&           browseName,
    const ReferenceTypeAttributes& attr,
    NodeId&                        outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addReferenceTypeNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addDataTypeNode(
    const NodeId&             nodeId,
    const NodeId&             parent,
    const NodeId&             referenceTypeId,
    const QualifiedName&      browseName,
    const DataTypeAttributes& attr,
    NodeId&                   outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addDataTypeNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addMethodNode(
    const NodeId&             nodeId,
    const NodeId&             parent,
    const NodeId&             referenceTypeId,
    const QualifiedName&      browseName,
    const MethodAttributes&   attr,
    NodeId&                   outNewNodeId /*= NodeId::Null*/) {
    if (!m_pClient) return false;
    WriteLock l(m_mutex);
    m_lastError = UA_Client_addMethodNode(
        m_pClient,
        nodeId,
        parent,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

UA_Boolean Client::historicalIteratorCallback(
    UA_Client*                  client,
    const UA_NodeId*            nodeId,
    UA_Boolean                  moreDataAvailable,
    const UA_ExtensionObject*   data,
    void*                       callbackContext) {
    if (callbackContext && nodeId && data) {
        if (auto p = (Client*)callbackContext)
            if (p->historicalIterator(NodeId(*nodeId), moreDataAvailable, *data))
                return UA_TRUE;
    }
    return UA_FALSE;
}

//*****************************************************************************

bool Client::historyReadRaw(
    const NodeId&       node,
    UA_DateTime         startTime,
    UA_DateTime         endTime,
    unsigned            numValuesPerNode,
    const UA_String&    indexRange              /*= UA_STRING_NULL*/,
    bool                returnBounds            /*= false*/,
    UA_TimestampsToReturn timestampsToReturn    /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    m_lastError = UA_Client_HistoryRead_raw(
        m_pClient,
        node.constRef(),
        historicalIteratorCallback,
        startTime,
        endTime,
        indexRange,
        returnBounds ? UA_TRUE : UA_FALSE, (UA_UInt32)numValuesPerNode,
        timestampsToReturn,
        this);
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateInsert(const NodeId& node, const UA_DataValue& value) {
    m_lastError = UA_Client_HistoryUpdate_insert(
        m_pClient,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateReplace(const NodeId& node, const UA_DataValue& value) {
    m_lastError = UA_Client_HistoryUpdate_replace(
        m_pClient,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateUpdate(const NodeId& node, const UA_DataValue& value) {
    m_lastError = UA_Client_HistoryUpdate_update(
        m_pClient,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateDeleteRaw(
    const NodeId&   node,
    UA_DateTime     startTimestamp,
    UA_DateTime     endTimestamp) {
    m_lastError = UA_Client_HistoryUpdate_deleteRaw(
        m_pClient,
        node.constRef(),
        startTimestamp,
        endTimestamp);
    return lastOK();
}

} // namespace Open62541

