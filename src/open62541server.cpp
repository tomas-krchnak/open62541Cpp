/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include "open62541server.h"
#include "nodecontext.h"
#include "serverbrowser.h"
#include "open62541client.h"
#include "historydatabase.h"

namespace Open62541 {

// map UA_SERVER to Server objects
Server::ServerMap Server::_serverMap;

//*****************************************************************************

// Life-cycle call backs
UA_StatusCode Server::constructor(
    UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void** nodeContext) {
    if (!server || !nodeId || !nodeContext) return UA_STATUSCODE_GOOD;

    if (auto pContext = (NodeContext*)(*nodeContext)) {
        if (Server* pServer = Server::findServer(server)) {
            NodeId node(*nodeId);
            return (pContext->construct(*pServer, node)) ? UA_STATUSCODE_GOOD : UA_STATUSCODE_BADINTERNALERROR;
        }
    }
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

void Server::destructor(
    UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext) {
    if (!server || !nodeId || !nodeContext) return;
    
    if (Server* pServer = Server::findServer(server)) {
        NodeId node(*nodeId);
        ((NodeContext*)nodeContext)->destruct(*pServer, node);
    }
}

//*****************************************************************************

// Access Control Callbacks - these invoke virtual functions to control access
UA_Boolean
Server::allowAddNodeHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_AddNodesItem* item) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowAddNode(accessControl, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_Boolean
Server::allowAddReferenceHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_AddReferencesItem* item) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowAddReference(accessControl, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_Boolean
Server::allowDeleteNodeHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_DeleteNodesItem* item) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowDeleteNode(accessControl, sessionId, sessionContext, item);
    }
    return UA_FALSE; // Do not allow deletion from client
}

//*****************************************************************************

UA_Boolean
Server::allowDeleteReferenceHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_DeleteReferencesItem* item) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowDeleteReference(accessControl, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_StatusCode Server::activateSessionHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_EndpointDescription* endpointDescription,
    const UA_ByteString* secureChannelRemoteCertificate,
    const UA_NodeId* sessionId,
    const UA_ExtensionObject* userIdentityToken,
    void** sessionContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->activateSession(accessControl, endpointDescription, secureChannelRemoteCertificate,
                                  sessionId, userIdentityToken,   sessionContext);
    }
    return -1;
}

//*****************************************************************************

void Server::closeSessionHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext) {
    if (Server* pServer = Server::findServer(server)) {
        pServer->closeSession(accessControl, sessionId, sessionContext);
    }
}

//*****************************************************************************

UA_UInt32 Server::getUserRightsMaskHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserRightsMask(accessControl, sessionId, sessionContext, nodeId, nodeContext);
    }
    return 0;
}

//*****************************************************************************

UA_Byte Server::getUserAccessLevelHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserAccessLevel(accessControl, sessionId, sessionContext, nodeId, nodeContext);
    }
    return 0;
}

//*****************************************************************************

UA_Boolean Server::getUserExecutableHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* methodId, void* methodContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserExecutable(accessControl, sessionId, sessionContext, methodId, methodContext);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_Boolean Server::getUserExecutableOnObjectHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* methodId, void* methodContext,
    const UA_NodeId* objectId, void* objectContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserExecutableOnObject(accessControl, sessionId, sessionContext,
                                            methodId, methodContext, objectId, objectContext);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_Boolean Server::allowHistoryUpdateUpdateDataHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId,
    UA_PerformUpdateType performInsertReplace,
    const UA_DataValue* value) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowHistoryUpdateUpdateData(accessControl, sessionId, sessionContext, nodeId,
                                               performInsertReplace, value);
    }
    return UA_FALSE;
}

//*****************************************************************************

UA_Boolean Server::allowHistoryUpdateDeleteRawModifiedHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId,
    UA_DateTime startTimestamp,
    UA_DateTime endTimestamp,
    bool isDeleteModified) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->allowHistoryUpdateDeleteRawModified(accessControl, sessionId, sessionContext, nodeId,
                                                      startTimestamp, endTimestamp, isDeleteModified);
    }
    return UA_FALSE;
}

//*****************************************************************************

Server::Server() {
    if (_server = UA_Server_new()) {
        if (_config = UA_Server_getConfig(_server)) {
            UA_ServerConfig_setDefault(_config);
            _config->nodeLifecycle.constructor = constructor; // set up the node global lifecycle
            _config->nodeLifecycle.destructor = destructor;
        }
    }
}

//*****************************************************************************

Server::Server(
    int port,
    const UA_ByteString& certificate /*= UA_BYTESTRING_NULL*/) {

    if (_server = UA_Server_new()) {
        if (_config = UA_Server_getConfig(_server)) {
            UA_ServerConfig_setMinimal(_config, port, &certificate);
            _config->nodeLifecycle.constructor = constructor; // set up the node global lifecycle
            _config->nodeLifecycle.destructor = destructor;
        }
    }
}

//*****************************************************************************

bool Server::enableSimpleLogin() {
    if (_logins.size() < 1 || !_config) return false;
    
    // Disable anonymous logins, enable two user/password logins
    _config->accessControl.deleteMembers(&_config->accessControl);
    UA_StatusCode retval = UA_AccessControl_default(_config, false,
                                                    &_config->securityPolicies[_config->securityPoliciesSize - 1].policyUri,
                                                    _logins.size(),
                                                    _logins.data());
    if (retval != UA_STATUSCODE_GOOD) return false;
    
    // Set accessControl functions for nodeManagement - these call virtual functions in the server object
    _config->accessControl.allowAddNode = Server::allowAddNodeHandler;
    _config->accessControl.allowAddReference = Server::allowAddReferenceHandler;
    _config->accessControl.allowDeleteNode = Server::allowDeleteNodeHandler;
    _config->accessControl.allowDeleteReference = Server::allowDeleteReferenceHandler;
    return true;
}

//*****************************************************************************

bool Server::deleteTree(NodeId& nodeId) {
    if (!_server) return false;

    NodeIdMap nodeMap; // set of nodes to delete
    browseTree(nodeId, nodeMap);
    for (auto& node : nodeMap) {
        if (node.second.namespaceIndex < 1) continue; // namespaces 0 appears to be reserved

        WriteLock l(_mutex);
        UA_Server_deleteNode(_server, node.second, true);
    }
    return lastOK();
}

/******************************************************************************
 * browseTreeCallBack
 * @param childId
 * @param isInverse
 * @param referenceTypeId
 * @param handle
 * @return 
 */
static UA_StatusCode browseTreeCallBack(
    UA_NodeId   childId,
    UA_Boolean  isInverse,
    UA_NodeId   referenceTypeId,
    void*       handle) {
    if (!isInverse) { // not a parent node - only browse forward
        ((UANodeIdList*)handle)->put(childId);
    }
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Server::browseChildren(UA_NodeId& nodeId, NodeIdMap& nodeMap) {
    if (!_server) return false;

    UANodeIdList children;
    {
        WriteLock ll(_mutex);
        UA_Server_forEachChildNodeCall(_server, nodeId, browseTreeCallBack, &children); // get the child list
    }
    for (auto& child : children) {
        if (child.namespaceIndex != nodeId.namespaceIndex) continue; // only in same namespace
        
        std::string s = toString(child);
        if (nodeMap.find(s) == nodeMap.end()) {
            nodeMap.put(child);
            browseChildren(child, nodeMap); // recurse no duplicates
        }
    }
    return lastOK();
}

//*****************************************************************************

bool Server::browseTree(UA_NodeId& nodeId, UANode* node) {
    if (!_server) return false;

    // form a hierarchical tree of nodes
    UANodeIdList children; // shallow copy node IDs and take ownership
    {   WriteLock ll(_mutex);
        // get the child list
        UA_Server_forEachChildNodeCall(_server, nodeId, browseTreeCallBack, &children);
    }
    for (auto& child : children) {
        if (child.namespaceIndex < 1) continue;
        
        QualifiedName outBrowseName;
        {   WriteLock ll(_mutex);
            _lastError = __UA_Server_read(_server, &child, UA_ATTRIBUTEID_BROWSENAME, outBrowseName);
        }
        if (_lastError != UA_STATUSCODE_GOOD) continue;

        std::string s = toString(outBrowseName.get().name);
        NodeId dataCopy = child;                // deep copy
        UANode* newNode = node->createChild(s); // create the node in the tree using the browse name as key
        newNode->setData(dataCopy);
        browseTree(child, newNode);             // recurse
    }
    return lastOK();
}

//*****************************************************************************

bool Server::browseTree(NodeId& nodeId, NodeIdMap& nodeMap) {
    nodeMap.put(nodeId);
    return browseChildren(nodeId, nodeMap);
}

//*****************************************************************************

void Server::terminate() {
    if (!_server) return;
    
    UA_Server_run_shutdown(_server);
    UA_Server_delete(_server);
    _serverMap.erase(_server);
    _server = nullptr;
}

//*****************************************************************************

void Server::start() {
    if (_running) return;
    
    _running = true;
    if (_server) {
        _serverMap[_server] = this; // map for call backs
        UA_Server_run_startup(_server);
        initialise();
        while (_running) {
            UA_Server_run_iterate(_server, true);
            process(); // called from time to time - Only safe places to access server are in process() and callbacks
        }
        terminate();
    }
    _running = false;
}

//*****************************************************************************

bool Server::nodeIdFromPath(
    NodeId& start,
    Path&   path,
    NodeId& nodeId) {
    nodeId = start;
    int level = 0;

    if (path.size() > 0) {
        ServerBrowser browser(*this);
        while (level < int(path.size())) {
            browser.browse(nodeId);
            auto itNode = browser.find(path[level]);
            if (itNode == browser.list().end()) return false;
            level++;
            nodeId = (*itNode).childId;
        }
    }
    return (level == int(path.size()));
}

//*****************************************************************************

bool Server::createFolderPath(
    NodeId& start,
    Path&   path,
    int     nameSpaceIndex,
    NodeId& nodeId) {
    UA_NodeId node = start.get(); // use node ids to browse with
    int level = 0;

    if (path.size() > 0) {
        ServerBrowser browser(*this);
        while (level < int(path.size())) {
            browser.browse(node);
            auto itNode = browser.find(path[level]);
            if (itNode == browser.list().end())  break;
            level++;
            node = (*itNode).childId; // shallow copy
        }
        nodeId = node;
        NodeId newNode;
        while (level < int(path.size())) {
            if (!addFolder(nodeId, path[level], NodeId::Null, newNode.notNull(), nameSpaceIndex)) break;
            nodeId = newNode; // assign
            level++;
        }
    }
    return (level == int(path.size()));
}

//*****************************************************************************

bool Server::getChild(
    NodeId&             start,
    const std::string&  childName,
    NodeId&             ret) {
    Path path;
    path.push_back(childName);
    return nodeIdFromPath(start, path, ret);
}

//*****************************************************************************

bool Server::addFolder(
    NodeId&             parent,
    const std::string&  browseName,
    NodeId&             nodeId,
    NodeId&             newNode         /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/) {

    if (!_server) return false;
    if (nameSpaceIndex == 0) nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    ObjectAttributes attr;
    attr.setDefault();
    attr.setDisplayName(browseName);
    attr.setDescription(browseName);

    WriteLock l(_mutex);
    _lastError = UA_Server_addObjectNode(
        _server,
        nodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, browseName),
        NodeId::FolderType,
        attr.get(),
        NULL,
        newNode.isNull() ? nullptr : newNode.ref());

    return lastOK();
}

//*****************************************************************************

bool Server::addVariable(
    NodeId&             parent,
    const std::string&  browseName,
    Variant&            value,
    NodeId&             nodeId,
    NodeId&             newNode         /*= NodeId::*/,
    NodeContext*        context         /*= nullptr*/,
    int                 nameSpaceIndex  /*= 0*/) {

    if (!_server) return false;
    if (nameSpaceIndex == 0) nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(browseName);
    var_attr.setDescription(browseName);
    var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    var_attr.setValue(value);
    var_attr.get().dataType = value.get().type->typeId;

    WriteLock l(_mutex);
    _lastError = UA_Server_addVariableNode(
        _server,
        nodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, browseName),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        var_attr,
        context,
        newNode.isNull() ? nullptr : newNode.ref());

    return lastOK();
}

//*****************************************************************************

bool Server::addHistoricalVariable(
    NodeId&         parent,
    const std::string& broseName,
    Variant&        value,
    NodeId&         nodeId,
    NodeId&         newNode         /*= NodeId::Null*/,
    NodeContext*    context         /*= nullptr*/,
    int             nameSpaceIndex  /*= 0*/) {

    if (!_server) return false;
    if (nameSpaceIndex == 0) nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(broseName);
    var_attr.setDescription(broseName);
    var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYREAD;
    var_attr.setValue(value);
    var_attr.get().dataType = value.get().type->typeId;
    var_attr.get().historizing = true;

    WriteLock l(_mutex);
    _lastError = UA_Server_addVariableNode(
        _server,
        nodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(nameSpaceIndex, broseName),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        var_attr,
        context,
        newNode.isNull() ? nullptr : newNode.ref());

    return lastOK();
}

//*****************************************************************************

bool Server::addProperty(
    NodeId&         parent,
    const std::string& key,
    Variant&        value,
    NodeId&         nodeId          /*= NodeId::Null*/,
    NodeId&         newNode         /*= NodeId::Null*/,
    NodeContext*    context         /*= nullptr*/,
    int             nameSpaceIndex  /*= 0*/) {
    if (!_server) return false;

    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(key);
    var_attr.setDescription(key);
    var_attr.get().accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    var_attr.setValue(value);
    _lastError = UA_Server_addVariableNode(
        _server,
        nodeId,
        parent,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        QualifiedName(nameSpaceIndex, key),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        var_attr,
        context,
        newNode.isNull() ? nullptr : newNode.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addServerMethod(
    ServerMethod*       method,
    const std::string&  browseName,
    NodeId&             parent,
    NodeId&             nodeId,
    NodeId&             newNode         /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/) {

    if (!server()) return false;
    if (nameSpaceIndex == 0) nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    MethodAttributes attr;
    attr.setDefault();
    attr.setDisplayName(browseName);
    attr.setDescription(browseName);
    attr.setExecutable();

    WriteLock l(mutex());
    _lastError = UA_Server_addMethodNode(
        _server,
        nodeId,
        parent,
        NodeId::HasOrderedComponent,
        QualifiedName(nameSpaceIndex, browseName),
        attr,
        ServerMethod::methodCallback,
        method->in().size() - 1,
        method->in().data(),
        method->out().size() - 1,
        method->out().data(),
        (void*)(method), // method context is reference to the call handler
        newNode.isNull() ? nullptr : newNode.ref());

    return lastOK();
}

//*****************************************************************************

void Server::serverOnNetworkCallback(
    const UA_ServerOnNetwork*   serverNetwork,
    UA_Boolean                  isServerAnnounce,
    UA_Boolean                  isTxtReceived,
    void*                       data) {
    if (auto pServer = (Server*)(data))
        pServer->serverOnNetwork(serverNetwork, isServerAnnounce, isTxtReceived);
}

//*****************************************************************************

void Server::registerServerCallback(
    const UA_RegisteredServer*  registeredServer,
    void*                       data) {
    if (auto pServer = (Server*)(data))
        pServer->registerServer(registeredServer);
}

//*****************************************************************************

bool Server::registerDiscovery(
    Client&             client,
    const std::string&  semaphoreFilePath) {
    _lastError = UA_Server_register_discovery(_server, client.client(), semaphoreFilePath.empty() ? nullptr : semaphoreFilePath.c_str());
    return lastOK();
}

//*****************************************************************************

bool Server::unregisterDiscovery(Client& client) {
    if (!server()) return false;

    _lastError = UA_Server_unregister_discovery(server(), client.client());
    return lastOK();
}

//*****************************************************************************

bool Server::addPeriodicServerRegister(
    const std::string&  discoveryServerUrl,
    Client&             client,
    UA_UInt64&          periodicCallbackId,
    UA_UInt32           intervalMs              /*= 600*1000*/,
    UA_UInt32           delayFirstRegisterMs    /*= 1000*/) {
    if (!server()) return false;

    _lastError = UA_Server_addPeriodicServerRegisterCallback(
        server(),
        client.client(),
        discoveryServerUrl.c_str(),
        intervalMs,
        delayFirstRegisterMs,
        &periodicCallbackId);

    if (lastOK()) {
        _discoveryList[periodicCallbackId]  = discoveryServerUrl;
    }

    return lastOK();
}

} // namespace Open62541
