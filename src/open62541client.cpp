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
    if (_client) {
        disconnect();
        UA_Client_delete(_client);
    }
}

//*****************************************************************************

bool Client::runIterate(uint32_t interval /*= 100*/) {
    if (!_client) return false;

    _lastError = UA_Client_run_iterate(_client, interval);
    return lastOK();
}

//*****************************************************************************

void Client::initialise() {
    if (_client) {
        if (getState() != UA_CLIENTSTATE_DISCONNECTED) disconnect();
        UA_Client_delete(_client);
        _client = nullptr;
    }
    _client = UA_Client_new();
    if (!_client)
        return;

    UA_ClientConfig_setDefault(UA_Client_getConfig(_client)); // initalise the client structure
    UA_Client_getConfig(_client)->clientContext = this;
    UA_Client_getConfig(_client)->stateCallback = stateCallback;
    UA_Client_getConfig(_client)->subscriptionInactivityCallback = subscriptionInactivityCallback;
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
        _subscriptions[newId] = sub;
        return true;
    }

    return false;
}

//*****************************************************************************

bool Client::removeSubscription(UA_UInt32 Id) {
    _subscriptions.erase(Id); // remove from dictionary implicit delete
    return true;
}

//*****************************************************************************

ClientSubscription* Client::subscription(UA_UInt32 Id) {
    if (_subscriptions.find(Id) != _subscriptions.end()) {
        ClientSubscriptionRef& c = _subscriptions[Id];
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
    if (!_client) return false;

    UA_EndpointDescription* endpointDescriptions     = nullptr;
    size_t                  endpointDescriptionsSize = 0;
    {
        WriteLock l(_mutex);
        _lastError = UA_Client_getEndpoints(
            _client, serverUrl.c_str(),
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
    if (!_client) {
        throw std::runtime_error("Null client");
        return 0;
    }

    EndpointDescriptionArray endpoints;
    if (!getEndpoints(serverUrl, endpoints))
        return _lastError;

    for (const auto& descr : endpoints)
        list.push_back(toString(descr.endpointUrl));

    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Client::findServers(
    const std::string&           serverUrl,
    StringArray&                 serverUris,
    StringArray&                 localeIds,
    ApplicationDescriptionArray& registeredServers) {
    if (!_client) return false;

    WriteLock l(_mutex);
    _lastError = UA_Client_findServers(
        _client,
        serverUrl.c_str(),
        serverUris.size(),
        serverUris.data(),
        localeIds.size(),
        localeIds.data(),
        registeredServers.lengthRef(),
        registeredServers.dataRef());
    UAPRINTLASTERROR(_lastError)
        return lastOK();
}

//*****************************************************************************

bool Client::findServersOnNetwork(
    const std::string&      serverUrl,
    unsigned                startingRecordId,
    unsigned                maxRecordsToReturn,
    StringArray&            serverCapabilityFilter,
    ServerOnNetworkArray&   serverOnNetwork) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_findServersOnNetwork(
        _client, serverUrl.c_str(),
        startingRecordId,
        maxRecordsToReturn,
        serverCapabilityFilter.size(),
        serverCapabilityFilter.data(),
        serverOnNetwork.lengthRef(),
        serverOnNetwork.dataRef());
    return lastOK();
}

//*****************************************************************************

bool Client::readAttribute(
    const UA_NodeId*    nodeId,
    UA_AttributeId      attr,
    void*               outVal,
    const UA_DataType*  outType) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = __UA_Client_readAttribute(_client, nodeId, attr, outVal, outType);
    return lastOK();
}

//*****************************************************************************

bool Client::writeAttribute(
    const UA_NodeId*    nodeId,
    UA_AttributeId      attr,
    const void*         val,
    const UA_DataType*  type) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = __UA_Client_writeAttribute(_client, nodeId, attr, val, type);
    return lastOK();
}

//*****************************************************************************

UA_ClientState Client::getState() {
    ReadLock l(_mutex);
    if (_client) return UA_Client_getState(_client);
    throw std::runtime_error("Null client");
    return UA_CLIENTSTATE_DISCONNECTED;
}

//*****************************************************************************

void Client::reset() {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    UA_Client_reset(_client);
    return;
}

//*****************************************************************************

bool Client::connect(const std::string& endpointUrl) {
    initialise();
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_connect(_client, endpointUrl.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::connectUsername(
    const std::string& endpoint,
    const std::string& username,
    const std::string& password) {
    initialise();
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_connect_username(
        _client,
        endpoint.c_str(),
        username.c_str(),
        password.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::connectAsync(const std::string& endpoint) {
    initialise();
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_connect_async(
        _client,
        endpoint.c_str(),
        asyncConnectCallback,
        this);
    return lastOK();
}

//*****************************************************************************

bool Client::connectNoSession(const std::string& endpoint) {
    initialise();
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_connect_noSession(_client, endpoint.c_str());
    return lastOK();
}

//*****************************************************************************

bool Client::disconnect() {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_disconnect(_client);
    return lastOK();
}

//*****************************************************************************

bool Client::disconnectAsync(UA_UInt32 requestId /*= 0*/) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_disconnect_async(_client, &requestId);
    return lastOK();
}

//*****************************************************************************

int Client::namespaceGetIndex(const std::string& namespaceUri) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    int namespaceIndex = 0;
    UA_String uri = toUA_String(namespaceUri);
    if (UA_Client_NamespaceGetIndex(
        _client,
        &uri,
        (UA_UInt16*)(&namespaceIndex)) == UA_STATUSCODE_GOOD) {
        return namespaceIndex;
    }
    return -1; // value
}

//*****************************************************************************

bool Client::browseName(const NodeId& nodeId, std::string& outName, int& outNamespace) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    QualifiedName outBrowseName;
    _lastError = UA_Client_readBrowseNameAttribute(_client, nodeId, outBrowseName);
    if (_lastError == UA_STATUSCODE_GOOD) {
        outName = toString(outBrowseName.get().name);
        outNamespace = outBrowseName.get().namespaceIndex;
    }
    return _lastError == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

void Client::setBrowseName(NodeId& nodeId, int nameSpaceIndex, const std::string& name) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    QualifiedName newBrowseName(nameSpaceIndex, name);
    UA_Client_writeBrowseNameAttribute(_client, nodeId, newBrowseName);
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
    WriteLock ll(_mutex);

    UA_Client_forEachChildNodeCall(
        _client, node,
        browseTreeCallBack, // browse the tree
        &children);         // output arg of the call-back. hold the list

    return children; // NRVO
}

//*****************************************************************************

bool Client::browseTree(UA_NodeId& nodeId, UANode* node) {
    if (!_client) return false;
    
    for (auto& child : getChildrenList(nodeId)) {
        if (child.namespaceIndex < 1) continue;

        QualifiedName outBrowseName;
        if (!readBrowseNameAttribute(child, outBrowseName)) continue;
        
        std::string s = toString(outBrowseName.name());
        NodeId dataCopy = child;        // deep copy
        // create the node in the tree using the browse name as key
        UANode* pNewNode = node->createChild(s);
        pNewNode->setData(dataCopy);
        browseTree(child, pNewNode);    // recurse
    }
    return lastOK();
}

//*****************************************************************************

bool Client::browseTree(NodeId& nodeId, UANodeTree& outTree) {
    // form a hierarchical tree of nodes. given node is added to tree
    outTree.root().setData(nodeId); // set the root of the tree
    return browseTree(nodeId.get(), outTree.rootNode());
}

//*****************************************************************************

bool Client::browseTree(NodeId& nodeId, NodeIdMap& outNodeMap) {
    outNodeMap.put(nodeId);
    return browseChildren(nodeId, outNodeMap);
}

//*****************************************************************************

bool Client::browseChildren(UA_NodeId& nodeId, NodeIdMap& nodeMap) {
    for (auto& child : getChildrenList(nodeId)) {
        if (child.namespaceIndex != nodeId.namespaceIndex)
            continue; // only in same namespace

        std::string s = toString(child);
        if (nodeMap.find(s) == nodeMap.end()) {
            nodeMap.put(child);
            browseChildren(child, nodeMap); // recurse no duplicates
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Client::nodeIdFromPath(NodeId& start, Path& path, NodeId& nodeId) {
    // nodeId is a shallow copy - do not delete and is volatile
    UA_NodeId node = start.get();

    int level = 0;
    if (path.size() > 0) {
        ClientBrowser browser(*this);
        while (level < int(path.size())) {
            browser.browse(node);
            auto it = browser.find(path[level]);
            if (it == browser.list().end()) return false;
            level++;
            node = (*it).nodeId;
        }
    }

    nodeId = node; // deep copy
    return level == int(path.size());
}

//*****************************************************************************

bool Client::createFolderPath(
    NodeId& start,
    Path&   path,
    int     nameSpaceIndex,
    NodeId& nodeId) {

    if (path.size() < 1)
        return true;

    UA_NodeId node = start.get();
    int level = 0;
    ClientBrowser browser(*this);

    while (level < int(path.size())) {
        browser.browse(node);
        auto it = browser.find(path[level]);
        if (it == browser.list().end())  break;
        level++;
        node = (*it).nodeId; // shallow copy
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

bool Client::getChild(NodeId& start, const std::string& childName, NodeId& ret) {
    Path path;
    path.push_back(childName);
    return nodeIdFromPath(start, path, ret);
}

//*****************************************************************************

bool Client::readArrayDimensionsAttribute(
    const UA_NodeId&        nodeId,
    std::vector<UA_UInt32>& ret) {
    if (!_client) return false;

    WriteLock l(_mutex);
    size_t      outArrayDimensionsSize  = 0;
    UA_UInt32*  outArrayDimensions      = nullptr;
    _lastError = UA_Client_readArrayDimensionsAttribute(
        _client,
        nodeId,
        &outArrayDimensionsSize,
        &outArrayDimensions);

    if (_lastError == UA_STATUSCODE_GOOD) {
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

bool Client::setArrayDimensionsAttribute(
    NodeId&                 nodeId,
    std::vector<UA_UInt32>& newArrayDimensions) {
    _lastError = UA_Client_writeArrayDimensionsAttribute(
        _client,
        nodeId,
        UA_UInt32(newArrayDimensions.size()),
        newArrayDimensions.data());
    return lastOK();
}

//*****************************************************************************

bool Client::variable(const NodeId& nodeId, Variant& value) {
    if (!_client) return false;
    WriteLock l(_mutex);
    // outValue is managed by caller - transfer to output value
    value.clear();
    _lastError = UA_Client_readValueAttribute(_client, nodeId, value); // shallow copy
    return lastOK();
}

//*****************************************************************************

bool Client::nodeClass(NodeId& nodeId, NodeClass& c) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_readNodeClassAttribute(_client, nodeId, &c);
    return lastOK();
}

//*****************************************************************************

bool Client::deleteNode(NodeId& nodeId, bool deleteReferences) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");
    _lastError = UA_Client_deleteNode(_client, nodeId, UA_Boolean(deleteReferences));
    return lastOK();
}

//*****************************************************************************

bool Client::deleteTree(NodeId& nodeId) {
    if (!_client) return false;

    NodeIdMap nodeMap;
    browseTree(nodeId, nodeMap);
    for (auto& node : nodeMap) {
        if (node.second.namespaceIndex > 0) { // namespace 0 appears to be reserved
            WriteLock l(_mutex);
            UA_Client_deleteNode(_client, node.second, true);
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Client::callMethod(
    NodeId&         objectId,
    NodeId&         methodId,
    VariantList&    in,
    VariantArray&   out) {
    WriteLock l(_mutex);
    if (!_client) throw std::runtime_error("Null client");

    size_t      outputSize  = 0;
    UA_Variant* output      = nullptr;

    _lastError = UA_Client_call(
        _client,
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

bool Client::setVariable(NodeId& nodeId, const Variant& value) {
    if (!_client) return false;
    _lastError = UA_Client_writeValueAttribute(_client, nodeId, value);
    return lastOK();
}

//*****************************************************************************

bool Client::addFolder(
    NodeId&             parent,
    const std::string&  childName,
    NodeId&             nodeId,
    NodeId&             outNewNodeId     /*= NodeId::Null*/,
    int                 nameSpaceIndex   /*= 0*/) {
    if(!_client) return false;
    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    ObjectAttributes attr;
    attr.setDisplayName(childName);
    attr.setDescription(childName);
    _lastError = UA_Client_addObjectNode(
        _client,
        nodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, childName),
        NodeId::FolderType,
        attr.get(),
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addVariable(
    NodeId&             parent,
    const std::string&  childName,
    const Variant&      value,
    NodeId&             nodeId,
    NodeId&             outNewNodeId     /*= NodeId::Null*/,
    int                 nameSpaceIndex   /*= 0*/) {
    if(!_client) return false;
    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    var_attr.setDisplayName(childName);
    var_attr.setDescription(childName);
    var_attr.setValue(value);
    _lastError = UA_Client_addVariableNode(
        _client,
        nodeId, // Assign new/random NodeID
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, childName),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        var_attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addProperty(
    NodeId&             parent,
    const std::string&  key,
    Variant&            value,
    NodeId&             nodeId,
    NodeId&             outNewNodeId    /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/) {
    if(!_client) return false;
    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
        nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    var_attr.setDisplayName(key);
    var_attr.setDescription(key);
    var_attr.setValue(value);
    _lastError = UA_Client_addVariableNode(
        _client,
        nodeId, // Assign new/random NodeID
        parent,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        QualifiedName(nameSpaceIndex, key),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        var_attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());

    return lastOK();
}

//*****************************************************************************

bool Client::addVariableTypeNode(
    NodeId&                 requestedNewNodeId,
    NodeId&                 parentNodeId,
    NodeId&                 referenceTypeId,
    QualifiedName&          browseName,
    VariableTypeAttributes& attr,
    NodeId&                 outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addVariableTypeNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addObjectNode(
    NodeId&             requestedNewNodeId,
    NodeId&             parentNodeId,
    NodeId&             referenceTypeId,
    QualifiedName&      browseName,
    NodeId&             typeDefinition,
    ObjectAttributes&   attr,
    NodeId&             outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addObjectNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addObjectTypeNode(
    NodeId&                 requestedNewNodeId,
    NodeId&                 parentNodeId,
    NodeId&                 referenceTypeId,
    QualifiedName&          browseName,
    ObjectTypeAttributes&   attr,
    NodeId&                 outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addObjectTypeNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addViewNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    ViewAttributes& attr,
    NodeId&         outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addViewNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addReferenceTypeNode(
    NodeId&                  requestedNewNodeId,
    NodeId&                  parentNodeId,
    NodeId&                  referenceTypeId,
    QualifiedName&           browseName,
    ReferenceTypeAttributes& attr,
    NodeId&                  outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addReferenceTypeNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addDataTypeNode(
    NodeId&             requestedNewNodeId,
    NodeId&             parentNodeId,
    NodeId&             referenceTypeId,
    QualifiedName&      browseName,
    DataTypeAttributes& attr,
    NodeId&             outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addDataTypeNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Client::addMethodNode(
    NodeId&             requestedNewNodeId,
    NodeId&             parentNodeId,
    NodeId&             referenceTypeId,
    QualifiedName&      browseName,
    MethodAttributes&   attr,
    NodeId&             outNewNodeId /*= NodeId::Null*/) {
    if (!_client) return false;
    WriteLock l(_mutex);
    _lastError = UA_Client_addMethodNode(
        _client,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

void  Client::asyncServiceCallback(
    UA_Client*          client,
    void*               userdata,
    UA_UInt32           requestId,
    void*               response,
    const UA_DataType*  responseType) {
    if(auto p = (Client*)UA_Client_getContext(client)) {
        p->asyncService(userdata, requestId, response, responseType);
    }
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
    const NodeId& node,
    UA_DateTime startTime,
    UA_DateTime endTime,
    unsigned numValuesPerNode,
    const UA_String& indexRange              /*= UA_STRING_NULL*/,
    bool returnBounds                        /*= false*/,
    UA_TimestampsToReturn timestampsToReturn /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    _lastError = UA_Client_HistoryRead_raw(
        _client,
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
    _lastError = UA_Client_HistoryUpdate_insert(
        _client,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateReplace(const NodeId& node, const UA_DataValue& value) {
    _lastError = UA_Client_HistoryUpdate_replace(
        _client,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateUpdate(const NodeId& node, const UA_DataValue& value) {
    _lastError = UA_Client_HistoryUpdate_update(
        _client,
        node.constRef(),
        const_cast<UA_DataValue*>(&value));
    return lastOK();
}

//*****************************************************************************

bool Client::historyUpdateDeleteRaw(
    const NodeId&   node,
    UA_DateTime     startTimestamp,
    UA_DateTime     endTimestamp) {
    _lastError = UA_Client_HistoryUpdate_deleteRaw(
        _client,
        node.constRef(),
        startTimestamp,
        endTimestamp);
    return lastOK();
}

} // namespace Open62541

