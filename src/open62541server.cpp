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
            return (pContext->construct(*pServer, node))
                ? UA_STATUSCODE_GOOD
                : UA_STATUSCODE_BADINTERNALERROR;
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
        return pServer->allowAddNode(
            accessControl,
            sessionId,
            sessionContext,
            item);
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
        return pServer->allowAddReference(
            accessControl,
            sessionId,
            sessionContext,
            item);
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
        return pServer->allowDeleteNode(
            accessControl,
            sessionId,
            sessionContext,
            item);
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
        return pServer->allowDeleteReference(
            accessControl,
            sessionId,
            sessionContext,
            item);
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
        return pServer->activateSession(
            accessControl,
            endpointDescription,
            secureChannelRemoteCertificate,
            sessionId,
            userIdentityToken,
            sessionContext);
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
        return pServer->getUserRightsMask(
            accessControl,
            sessionId,
            sessionContext,
            nodeId,
            nodeContext);
    }
    return 0;
}

//*****************************************************************************

UA_Byte Server::getUserAccessLevelHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserAccessLevel(
            accessControl,
            sessionId,
            sessionContext,
            nodeId,
            nodeContext);
    }
    return 0;
}

//*****************************************************************************

UA_Boolean Server::getUserExecutableHandler(
    UA_Server* server, UA_AccessControl* accessControl,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* methodId, void* methodContext) {
    if (Server* pServer = Server::findServer(server)) {
        return pServer->getUserExecutable(
            accessControl,
            sessionId,
            sessionContext,
            methodId,
            methodContext);
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
        return pServer->getUserExecutableOnObject(
            accessControl,
            sessionId,
            sessionContext,
            methodId,
            methodContext,
            objectId,
            objectContext);
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
        return pServer->allowHistoryUpdateUpdateData(
            accessControl,
            sessionId,
            sessionContext,
            nodeId,
            performInsertReplace,
            value);
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
        return pServer->allowHistoryUpdateDeleteRawModified(
            accessControl,
            sessionId,
            sessionContext,
            nodeId,
            startTimestamp,
            endTimestamp,
            isDeleteModified);
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

Server::~Server() {
    if (_server) {
        WriteLock l(_mutex);
        terminate();
    }
}

//*****************************************************************************

void Server::terminate() {
    if (!_server) return;

    UA_Server_run_shutdown(_server);
    UA_Server_delete(_server);
    _serverMap.erase(_server); // unreachable by call-backs
    _server = nullptr;
}

//*****************************************************************************

void Server::start() {
    if (_running) return;

    _running = true;
    if (_server) {
        _serverMap[_server] = this; // map for call-backs
        UA_Server_run_startup(_server);
        initialise();
        while (_running) {
            UA_Server_run_iterate(_server, true);
            // called from time to time.
            // Only safe places to access server are in process() and callbacks
            process(); 
        }
        terminate();
    }
    _running = false;
}

//*****************************************************************************

void Server::applyEndpoints(EndpointDescriptionArray& endpoints) {
    _config->endpoints     = endpoints.data();
    _config->endpointsSize = endpoints.length();
    
    endpoints.release(); // Transfer ownership
}

//*****************************************************************************

bool Server::enableSimpleLogin() {
    if (_logins.size() < 1 || !_config) return false;
    
    // Disable anonymous logins, enable two user/password logins
    _config->accessControl.deleteMembers(&_config->accessControl);
    UA_StatusCode retval = UA_AccessControl_default(
        _config, false,
        &_config->securityPolicies[_config->securityPoliciesSize - 1].policyUri,
        _logins.size(),
        _logins.data());

    if (retval != UA_STATUSCODE_GOOD) return false;
    
    // Set accessControl functions for nodeManagement
    // these call virtual functions in the server object
    _config->accessControl.allowAddNode         = Server::allowAddNodeHandler;
    _config->accessControl.allowAddReference    = Server::allowAddReferenceHandler;
    _config->accessControl.allowDeleteNode      = Server::allowDeleteNodeHandler;
    _config->accessControl.allowDeleteReference = Server::allowDeleteReferenceHandler;
    return true;
}

//*****************************************************************************

void Server::setServerUri(const std::string& uri) {
    UA_String_deleteMembers(&_config->applicationDescription.applicationUri);
    _config->applicationDescription.applicationUri = UA_String_fromChars(uri.c_str());
}

//*****************************************************************************

void Server::setMdnsServerName(const std::string& name) {
    if (_config) {
        _config->discovery.mdnsEnable = true;

#ifdef UA_ENABLE_DISCOVERY_MULTICAST
        _config->discovery.mdns.mdnsServerName = UA_String_fromChars(name.c_str());
#else
        (void)name;
#endif
    }
}

//*****************************************************************************

bool Server::deleteTree(NodeId& nodeId) {
    if (!_server) return false;

    NodeIdMap nodeMap; // set of nodes to delete
    browseTree(nodeId, nodeMap);
    for (auto& node : nodeMap) {
        if (node.second.namespaceIndex < 1)
            continue; // namespaces 0 appears to be reserved

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
 * @return UA_STATUSCODE_GOOD
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
        UA_Server_forEachChildNodeCall( // get the child list
            _server, nodeId,
            browseTreeCallBack,
            &children);
    }
    for (auto& child : children) {
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

bool Server::browseSimplifiedBrowsePath(
    NodeId origin,
    size_t browsePathSize,
    QualifiedName& browsePath,
    BrowsePathResult& result) {
    result.get() = UA_Server_browseSimplifiedBrowsePath(
        _server,
        origin,
        browsePathSize,
        browsePath.constRef());
    _lastError = result.ref()->statusCode;
    return lastOK();
}

//*****************************************************************************

bool Server::browseTree(UA_NodeId& nodeId, UANode* node) {
    if (!_server) return false;

    // form a hierarchical tree of nodes
    UANodeIdList children; // shallow copy node IDs and take ownership
    {
        WriteLock ll(_mutex);
        UA_Server_forEachChildNodeCall( // get the child list
            _server, nodeId,
            browseTreeCallBack,
            &children);
    }
    for (auto& child : children) {
        if (child.namespaceIndex < 1) continue;
        
        QualifiedName outBrowseName;
        if (!readBrowseName(child, outBrowseName)) continue;

        std::string s = toString(outBrowseName.get().name);
        NodeId dataCopy = child;                // deep copy
        // create the node in the tree using the browse name as key
        UANode* newNode = node->createChild(s); 
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

bool Server::getNodeContext(NodeId& node, NodeContext*& pContext) {
    if (!server()) return false;

    void* p = (void*)pContext;
    _lastError = UA_Server_getNodeContext(_server, node.get(), &p);
    return lastOK();
}

//*****************************************************************************

bool Server::setNodeContext(NodeId& node, const NodeContext* context) {
    if (!server()) return false;

    _lastError = UA_Server_setNodeContext(_server, node.get(), (void*)context);
    return lastOK();
}

//*****************************************************************************

bool Server::browseName(NodeId& nodeId, std::string& name, int& idxNameSpace) {
    if (!_server) throw std::runtime_error("Null server");

    QualifiedName browseName;
    if (UA_Server_readBrowseName(_server, nodeId, browseName) == UA_STATUSCODE_GOOD) {
        name = toString(browseName.get().name);
        idxNameSpace  = browseName.get().namespaceIndex;
    }
    return lastOK();
}

//*****************************************************************************

void Server::setBrowseName(
    NodeId& nodeId,
    int nameSpaceIndex,
    const std::string& name) {
    if (!server()) return;

    QualifiedName newBrowseName(nameSpaceIndex, name);
    WriteLock l(_mutex);
    UA_Server_writeBrowseName(_server, nodeId, newBrowseName);
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
            if (!addFolder(nodeId,
                            path[level],
                            NodeId::Null,
                            newNode.notNull(),
                            nameSpaceIndex)) {
                break; // stop on failure
            }
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

UA_UInt16 Server::addNamespace(const std::string name) {
    if (!server()) return 0;

    WriteLock l(_mutex);
    return UA_Server_addNamespace(_server, name.c_str());
}

//*****************************************************************************

bool Server::addFolder(
    NodeId&             parent,
    const std::string&  browseName,
    NodeId&             nodeId,
    NodeId&             newNode         /*= NodeId::Null*/,
    int                 nameSpaceIndex  /*= 0*/) {

    if (!_server) return false;
    if (nameSpaceIndex == 0) // inherit parent by default
        nameSpaceIndex = parent.nameSpaceIndex();

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
    NodeId&             nodeId          /*= NodeId::Null*/,
    NodeId&             newNode         /*= NodeId::Null*/,
    NodeContext*        context         /*= nullptr*/,
    int                 nameSpaceIndex  /*= 0*/) {

    if (!_server) return false;
    if (nameSpaceIndex == 0) // inherit parent by default
        nameSpaceIndex = parent.nameSpaceIndex();

    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(browseName);
    var_attr.setDescription(browseName);
    var_attr.get().accessLevel  = UA_ACCESSLEVELMASK_READ
                                | UA_ACCESSLEVELMASK_WRITE;
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
    if (nameSpaceIndex == 0) // inherit parent by default
        nameSpaceIndex = parent.nameSpaceIndex();

    VariableAttributes var_attr;
    var_attr.setDefault();
    var_attr.setDisplayName(broseName);
    var_attr.setDescription(broseName);
    var_attr.get().accessLevel  = UA_ACCESSLEVELMASK_READ
                                | UA_ACCESSLEVELMASK_WRITE
                                | UA_ACCESSLEVELMASK_HISTORYREAD;
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
    var_attr.get().accessLevel  = UA_ACCESSLEVELMASK_READ
                                | UA_ACCESSLEVELMASK_WRITE;
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
    if (nameSpaceIndex == 0) // inherit parent by default
        nameSpaceIndex = parent.nameSpaceIndex();

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

bool Server::deleteNode(NodeId& nodeId, bool deleteReferences) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_deleteNode(_server, nodeId, UA_Boolean(deleteReferences));
    return lastOK();
}

//*****************************************************************************

bool Server::addVariableNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    NodeId&         typeDefinition,
    VariableAttributes& attr,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addVariableNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addVariableTypeNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    NodeId&         typeDefinition,
    VariableTypeAttributes& attr,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addVariableTypeNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addObjectNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    NodeId&         typeDefinition,
    ObjectAttributes& attr,
    NodeId&         outNewNodeId          /*= NodeId::Null*/,
    NodeContext*    instantiationCallback /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addObjectNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addObjectTypeNode(
    NodeId&             requestedNewNodeId,
    NodeId&             parentNodeId,
    NodeId&             referenceTypeId,
    QualifiedName&      browseName,
    ObjectTypeAttributes& attr,
    NodeId&             outNewNodeId            /*= NodeId::Null*/,
    NodeContext*        instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addObjectTypeNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addViewNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    ViewAttributes& attr,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addViewNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addReferenceTypeNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    ReferenceTypeAttributes& attr,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addReferenceTypeNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addDataTypeNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    DataTypeAttributes& attr,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addDataTypeNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        attr,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addDataSourceVariableNode(
    NodeId&         requestedNewNodeId,
    NodeId&         parentNodeId,
    NodeId&         referenceTypeId,
    QualifiedName&  browseName,
    NodeId&         typeDefinition,
    VariableAttributes& attr,
    DataSource&     dataSource,
    NodeId&         outNewNodeId            /*= NodeId::Null*/,
    NodeContext*    instantiationCallback   /*= nullptr*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addDataSourceVariableNode(
        _server,
        requestedNewNodeId,
        parentNodeId,
        referenceTypeId,
        browseName,
        typeDefinition,
        attr,
        dataSource,
        instantiationCallback,
        outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::addReference(
    NodeId&         sourceId,
    NodeId&         referenceTypeId,
    ExpandedNodeId& targetId,
    bool            isForward) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_addReference(
        server(),
        sourceId,
        referenceTypeId,
        targetId,
        isForward);
    return lastOK();
}

//*****************************************************************************

bool Server::markMandatory(NodeId& nodeId) {
    return addReference(
        nodeId,
        NodeId::HasModellingRule,
        ExpandedNodeId::ModellingRuleMandatory,
        true);
}

//*****************************************************************************

bool Server::deleteReference(
    NodeId&         sourceNodeId,
    NodeId&         referenceTypeId,
    bool            isForward,
    ExpandedNodeId& targetNodeId,
    bool            deleteBidirectional) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_deleteReference(
        server(),
        sourceNodeId,
        referenceTypeId,
        isForward,
        targetNodeId,
        deleteBidirectional);
    return lastOK();
}

//*****************************************************************************

bool Server::addInstance(
    const std::string&  name,
    NodeId&             requestedNewNodeId,
    NodeId&             parent,
    NodeId&             typeId,
    NodeId&             nodeId  /*= NodeId::Null*/,
    NodeContext*        context /*= nullptr*/) {
    if (!server()) return false;

    ObjectAttributes oAttr;
    oAttr.setDefault();
    oAttr.setDisplayName(name);

    return addObjectNode(
        requestedNewNodeId,
        parent,
        NodeId::Organizes,
        QualifiedName(parent.nameSpaceIndex(), name),
        typeId,
        oAttr,
        nodeId,
        context);
}

//*****************************************************************************

bool Server::createEvent(const NodeId& eventType, NodeId& outNodeId) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_createEvent(_server, eventType, outNodeId.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::triggerEvent(
    NodeId&         eventNodeId,
    UA_ByteString*  outEventId      /*= nullptr*/,
    bool            deleteEventNode /*= true*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_triggerEvent(
        _server,
        eventNodeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER),
        outEventId,
        deleteEventNode);
    return lastOK();
}

//*****************************************************************************

bool Server::addNewEventType(
    const std::string&  name,
    NodeId&             eventType,
    const std::string&  description /*= std::string()*/) {
    if (!server()) return false;

    ObjectTypeAttributes attr;
    attr.setDefault();
    attr.setDisplayName(name);
    attr.setDescription((description.empty() ? name : description));

    WriteLock l(_mutex);
    _lastError = UA_Server_addObjectTypeNode(
        server(),
        UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        QualifiedName(0, name),
        attr,
        NULL,
        eventType.ref());
    return lastOK();
}

//*****************************************************************************

bool Server::setUpEvent(
    NodeId&             outId,
    NodeId&             eventType,
    const std::string&  eventMessage,
    const std::string&  eventSourceName,
    int                 eventSeverity /*= 100*/,
    UA_DateTime         eventTime     /*= UA_DateTime_now()*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_createEvent(server(), eventType, outId);
    if (_lastError != UA_STATUSCODE_GOOD)
        return lastOK();

    /* Set the Event Attributes */
    /* Setting the Time is required or else the event will not show up in UAExpert! */
    UA_Server_writeObjectProperty_scalar(server(), outId,
        UA_QUALIFIEDNAME(0, const_cast<char*>("Time")),
        &eventTime, &UA_TYPES[UA_TYPES_DATETIME]);

    UA_Server_writeObjectProperty_scalar(server(), outId,
        UA_QUALIFIEDNAME(0, const_cast<char*>("Severity")),
        &eventSeverity, &UA_TYPES[UA_TYPES_UINT16]);

    LocalizedText eM(const_cast<char*>("en-US"), eventMessage);
    UA_Server_writeObjectProperty_scalar(server(), outId,
        UA_QUALIFIEDNAME(0, const_cast<char*>("Message")),
        eM.ref(), &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    UA_String eSN = UA_STRING(const_cast<char*>(eventSourceName.c_str()));
    UA_Server_writeObjectProperty_scalar(server(), outId,
        UA_QUALIFIEDNAME(0, const_cast<char*>("SourceName")),
        &eSN, &UA_TYPES[UA_TYPES_STRING]);

    return lastOK();
}

//*****************************************************************************

bool Server::call(CallMethodRequest& request, CallMethodResult& ret) {
    if (!server()) return false;

    WriteLock l(_mutex);
    ret.get() = UA_Server_call(_server, request);
    return ret.get().statusCode == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Server::translateBrowsePathToNodeIds(
    BrowsePath&         path,
    BrowsePathResult&   result) {
    if (!server()) return false;

    WriteLock l(_mutex);
    result.get() = UA_Server_translateBrowsePathToNodeIds(_server, path);
    return result.get().statusCode == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Server::readAttribute(
    const UA_NodeId* nodeId,
    UA_AttributeId   attributeId,
    void*            value) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = __UA_Server_read(_server, nodeId, attributeId, value);
    return lastOK();
}

//*****************************************************************************

bool Server::writeAttribute(
    const UA_NodeId*     nodeId,
    const UA_AttributeId attributeId,
    const UA_DataType*   attr_type,
    const void*          attr) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = __UA_Server_write(_server, nodeId, attributeId, attr_type, attr);
    return lastOK();
}

//*****************************************************************************

bool Server::writeEnable(NodeId& nodeId) {
    UA_Byte accessLevel;
    if (readAccessLevel(nodeId, accessLevel)) {
        accessLevel |= UA_ACCESSLEVELMASK_WRITE;
        return writeAccessLevel(nodeId, accessLevel);
    }
    return false;
}

//*****************************************************************************

bool Server::setReadOnly(NodeId& nodeId, bool historyEnable /*= false*/) {
    UA_Byte accessLevel;
    if (!readAccessLevel(nodeId, accessLevel))
        return false;

    // remove the write bits
    accessLevel &= ~(UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYWRITE);
    // add the read bits
    accessLevel |= UA_ACCESSLEVELMASK_READ;
    if (historyEnable) accessLevel |= UA_ACCESSLEVELMASK_HISTORYREAD;
    return writeAccessLevel(nodeId, accessLevel);
}

//*****************************************************************************

bool Server::updateCertificate(
    const UA_ByteString* oldCertificate,
    const UA_ByteString* newCertificate,
    const UA_ByteString* newPrivateKey,
    bool                 closeSessions       /*= true*/,
    bool                 closeSecureChannels /*= true*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    _lastError = UA_Server_updateCertificate(
        _server,
        oldCertificate,
        newCertificate,
        newPrivateKey,
        closeSessions,
        closeSecureChannels);
    return lastOK();
}

//*****************************************************************************

bool Server::accessControlAllowHistoryUpdateUpdateData(
    const NodeId&           sessionId,
    void*                   sessionContext,
    const NodeId&           nodeId,
    UA_PerformUpdateType    performInsertReplace,
    UA_DataValue&           value) {
    if (!server()) return false;

    WriteLock l(_mutex);
    return UA_Server_AccessControl_allowHistoryUpdateUpdateData(
        _server, sessionId.constRef(),
        sessionContext,
        nodeId.constRef(),
        performInsertReplace,
        &value) == UA_TRUE;
}

//*****************************************************************************

bool Server::accessControlAllowHistoryUpdateDeleteRawModified(
    const NodeId&   sessionId,
    void*           sessionContext,
    const NodeId&   nodeId,
    UA_DateTime     startTimestamp,
    UA_DateTime     endTimestamp,
    bool            isDeleteModified /*= true*/) {
    if (!server()) return false;

    WriteLock l(_mutex);
    return UA_Server_AccessControl_allowHistoryUpdateDeleteRawModified(
        _server,
        sessionId.constRef(), sessionContext,
        nodeId.constRef(),
        startTimestamp,
        endTimestamp,
        isDeleteModified);
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
    _lastError = UA_Server_register_discovery(
        _server,
        client.client(),
        semaphoreFilePath.empty() ? nullptr : semaphoreFilePath.c_str());

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
        _discoveryList[periodicCallbackId] = discoveryServerUrl;
    }

    return lastOK();
}

} // namespace Open62541
