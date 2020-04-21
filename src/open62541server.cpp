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
Server::ServerMap  Server::_serverMap;

NodeContext *Server::findContext(const std::string &s) {
    return RegisteredNodeContext::findRef(s); // not all node contexts are registered
}

void Server::setHistoryDatabase(UA_HistoryDatabase &h) {
    if (_config) _config->historyDatabase = h;
}

// Life-cycle call backs
UA_StatusCode Server::constructor(UA_Server *server,
                                const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
                                const UA_NodeId *nodeId, void **nodeContext) {
    UA_StatusCode ret = UA_STATUSCODE_GOOD;
    if (server && nodeId && nodeContext) {
        void *p = *nodeContext;
        NodeContext *cp = (NodeContext *)(p);
        if (cp) {
            Server *s = Server::findServer(server);
            if (s) {
                NodeId n(*nodeId);
                ret = (cp->construct(*s, n)) ? UA_STATUSCODE_GOOD : UA_STATUSCODE_BADINTERNALERROR;
            }
        }
    }
    return ret;
}

void Server::destructor(UA_Server *server,
                        const UA_NodeId* /*sessionId*/, void* /*sessionContext*/,
                        const UA_NodeId *nodeId, void *nodeContext) {
    if (server && nodeId && nodeContext) {
        NodeContext *cp = (NodeContext *)(nodeContext);
        Server *s = Server::findServer(server);
        if (s) {
            NodeId n(*nodeId);
            cp->destruct(*s, n);
        }
    }

}

// Access Control Callbacks - these invoke virtual functions to control access
UA_Boolean
Server::allowAddNodeHandler(UA_Server *server, UA_AccessControl *ac,
                            const UA_NodeId *sessionId, void *sessionContext,
                            const UA_AddNodesItem *item) {
    Server *p = Server::findServer(server); // find the server
    if (p) {
        return p->allowAddNode(ac, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}

UA_Boolean
Server::allowAddReferenceHandler(UA_Server *server, UA_AccessControl *ac,
                                const UA_NodeId *sessionId, void *sessionContext,
                                const UA_AddReferencesItem *item) {
    if (Server *p = Server::findServer(server)) {
        return p->allowAddReference(ac, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}

UA_Boolean
Server::allowDeleteNodeHandler(UA_Server *server, UA_AccessControl *ac,
                                const UA_NodeId *sessionId, void *sessionContext,
                                const UA_DeleteNodesItem *item) {
    if (Server *p = Server::findServer(server)) {
        return p->allowDeleteNode(ac, sessionId, sessionContext, item);
    }
    return UA_FALSE; // Do not allow deletion from client
}

UA_Boolean
Server::allowDeleteReferenceHandler(UA_Server *server, UA_AccessControl *ac,
                                    const UA_NodeId *sessionId, void *sessionContext,
                                    const UA_DeleteReferencesItem *item) {
    if (Server *p = Server::findServer(server)) {
        return p->allowDeleteReference(ac, sessionId, sessionContext, item);
    }
    return UA_FALSE;
}


UA_StatusCode Server::activateSessionHandler(UA_Server *server, UA_AccessControl *ac,
                                            const UA_EndpointDescription *endpointDescription,
                                            const UA_ByteString *secureChannelRemoteCertificate,
                                            const UA_NodeId *sessionId,
                                            const UA_ExtensionObject *userIdentityToken,
                                            void **sessionContext) {
    if (Server *p = Server::findServer(server)) {
        return p->activateSession(ac, endpointDescription, secureChannelRemoteCertificate,
                                  sessionId, userIdentityToken,   sessionContext);
    }
    return -1;
}

void Server::closeSessionHandler(UA_Server *server, UA_AccessControl *ac,
                                const UA_NodeId *sessionId, void *sessionContext) {
    if (Server *p = Server::findServer(server)) {
        p->closeSession(ac, sessionId, sessionContext);
    }
}

UA_UInt32 Server::getUserRightsMaskHandler(UA_Server *server, UA_AccessControl *ac,
                                            const UA_NodeId *sessionId, void *sessionContext,
                                            const UA_NodeId *nodeId, void *nodeContext) {
    if (Server *p = Server::findServer(server)) {
        return p->getUserRightsMask(ac, sessionId, sessionContext, nodeId, nodeContext);
    }
    return 0;
}

UA_Byte Server::getUserAccessLevelHandler(UA_Server *server, UA_AccessControl *ac,
                                            const UA_NodeId *sessionId, void *sessionContext,
                                            const UA_NodeId *nodeId, void *nodeContext) {
    if (Server *p = Server::findServer(server)) {
        return p->getUserAccessLevel(ac, sessionId, sessionContext, nodeId, nodeContext);
    }
    return 0;
}

UA_Boolean Server::getUserExecutableHandler(UA_Server *server, UA_AccessControl *ac,
                                            const UA_NodeId *sessionId, void *sessionContext,
                                            const UA_NodeId *methodId, void *methodContext) {
    if (Server *p = Server::findServer(server)) {
        return p->getUserExecutable(ac, sessionId, sessionContext, methodId, methodContext);
    }
    return UA_FALSE;
}

UA_Boolean Server::getUserExecutableOnObjectHandler(UA_Server *server, UA_AccessControl *ac,
                                                    const UA_NodeId *sessionId, void *sessionContext,
                                                    const UA_NodeId *methodId, void *methodContext,
                                                    const UA_NodeId *objectId, void *objectContext) {
    if (Server *p = Server::findServer(server)) {
        return p->getUserExecutableOnObject(ac, sessionId, sessionContext,
                                            methodId, methodContext, objectId, objectContext);
    }
    return UA_FALSE;
}

UA_Boolean Server::allowHistoryUpdateUpdateDataHandler(UA_Server *server, UA_AccessControl *ac,
                                                        const UA_NodeId *sessionId, void *sessionContext,
                                                        const UA_NodeId *nodeId,
                                                        UA_PerformUpdateType performInsertReplace,
                                                        const UA_DataValue *value) {
    if (Server *p = Server::findServer(server)) {
        return p->allowHistoryUpdateUpdateData(ac, sessionId, sessionContext, nodeId,
                                               performInsertReplace, value);
    }
    return UA_FALSE;
}

UA_Boolean Server::allowHistoryUpdateDeleteRawModifiedHandler(UA_Server *server, UA_AccessControl *ac,
                                                            const UA_NodeId *sessionId, void *sessionContext,
                                                            const UA_NodeId *nodeId,
                                                            UA_DateTime startTimestamp,
                                                            UA_DateTime endTimestamp,
                                                            bool isDeleteModified) {
    if (Server *p = Server::findServer(server)) {
        return p->allowHistoryUpdateDeleteRawModified(ac, sessionId, sessionContext, nodeId,
                                                      startTimestamp, endTimestamp, isDeleteModified);
    }
    return UA_FALSE;
}


Server::Server() {
    if (_server = UA_Server_new()) {
        if (_config = UA_Server_getConfig(_server)) {
            UA_ServerConfig_setDefault(_config);
            _config->nodeLifecycle.constructor = constructor; // set up the node global lifecycle
            _config->nodeLifecycle.destructor = destructor;
        }
    }
}


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

bool Server::enableSimpleLogin() {
    if ((_logins.size() > 0) && _config) {
        /* Disable anonymous logins, enable two user/password logins */
        _config->accessControl.deleteMembers(&_config->accessControl);
        UA_StatusCode retval = UA_AccessControl_default(_config, false,
                                                        &_config->securityPolicies[_config->securityPoliciesSize - 1].policyUri,
                                                        _logins.size(),
                                                        _logins.data());
        if (retval == UA_STATUSCODE_GOOD) {
            /* Set accessControl functions for nodeManagement - these call virtual functions in the server object*/
            _config->accessControl.allowAddNode = Server::allowAddNodeHandler;
            _config->accessControl.allowAddReference = Server::allowAddReferenceHandler;
            _config->accessControl.allowDeleteNode = Server::allowDeleteNodeHandler;
            _config->accessControl.allowDeleteReference = Server::allowDeleteReferenceHandler;
            return true;
        }
    }
    return false;
}

bool Server::deleteTree(NodeId &nodeId) {
    if (!_server) return false;
    NodeIdMap m; // set of nodes to delete
    browseTree(nodeId, m);
    for (auto i = m.begin(); i != m.end(); i++) {
        {
            UA_NodeId &ni =  i->second;
            if (ni.namespaceIndex > 0) { // namespaces 0  appears to be reserved
                WriteLock l(_mutex);
                UA_Server_deleteNode(_server, i->second, true);
            }
        }
    }
    return lastOK();
}

/**
 * browseTreeCallBack
 * @param childId
 * @param isInverse
 * @param referenceTypeId
 * @param handle
 * @return 
 */
static UA_StatusCode browseTreeCallBack(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId /*referenceTypeId*/, void *handle) {
    if (!isInverse) { // not a parent node - only browse forward
        UANodeIdList  *pl = (UANodeIdList *)handle;
        pl->put(childId);
    }
    return UA_STATUSCODE_GOOD;
}

bool Server::browseChildren(UA_NodeId &nodeId, NodeIdMap &m) {
    if (!_server) return false;
    UANodeIdList l;
    {

        WriteLock ll(_mutex);
        UA_Server_forEachChildNodeCall(_server, nodeId,  browseTreeCallBack, &l); // get the childlist
    }
    for (int i = 0; i < int(l.size()); i++) {
        if (l[i].namespaceIndex == nodeId.namespaceIndex) { // only in same namespace
            std::string s = toString(l[i]);
            if (m.find(s) == m.end()) {
                m.put(l[i]);
                browseChildren(l[i], m); // recurse no duplicates
            }
        }
    }
    return lastOK();
}

bool Server::browseTree(NodeId &nodeId, UANodeTree &tree) {
    // form a hierarchical tree of nodes given node is not added to tree
    return browseTree(nodeId.get(), tree.rootNode());
}

bool Server::browseTree(UA_NodeId &nodeId, UANode *node) {
    if (!_server) return false;
    // form a hierarchical tree of nodes
    UANodeIdList l; // shallow copy node IDs and take ownership
    {
        WriteLock ll(_mutex);
        UA_Server_forEachChildNodeCall(_server, nodeId,  browseTreeCallBack, &l); // get the child list
    }
    for (int i = 0; i < int(l.size()); i++) {
        if (l[i].namespaceIndex > 0) {
            QualifiedName outBrowseName;
            {
                WriteLock ll(_mutex);
                _lastError = __UA_Server_read(_server, &l[i], UA_ATTRIBUTEID_BROWSENAME, outBrowseName);
            }
            if (_lastError == UA_STATUSCODE_GOOD) {
                std::string s = toString(outBrowseName.get().name); // get the browse name and leak key
                NodeId nId = l[i]; // deep copy
                UANode *n = node->createChild(s); // create the node
                n->setData(nId);
                browseTree(l[i], n);
            }
        }
    }
    return lastOK();
}

bool Server::browseTree(NodeId &nodeId, NodeIdMap &m) {
    m.put(nodeId);
    return browseChildren(nodeId, m);
}

void Server::terminate() {
    if (_server) {
        //
        UA_Server_run_shutdown(_server);
        UA_Server_delete(_server);
        _serverMap.erase(_server);
        _server = nullptr;
    }
}

void Server::start() { // start the server
    if (!_running) {
        _running = true;
        if (_server) {
            _serverMap[_server] = this; // map for call backs
            UA_Server_run_startup(_server);
            initialise();
            while (_running) {
                {
                    UA_Server_run_iterate(_server, true);
                }
                process(); // called from time to time - Only safe places to access server are in process() and callbacks
            }
            terminate();
        }
        _running = false;
    }
}

void Server::stop() {
    _running = false;
}

bool Server::nodeIdFromPath(NodeId &start, Path &path,  NodeId &nodeId) {
    nodeId = start;
    int level = 0;

    if (path.size() > 0) {
        ServerBrowser b(*this);
        while (level < int(path.size())) {
            b.browse(nodeId);
            auto i = b.find(path[level]);
            if (i == b.list().end()) return false;
            level++;
            nodeId = (*i).childId;
        }
    }
    return level == int(path.size());
}



bool Server::createFolderPath(NodeId &start, Path &path, int nameSpaceIndex, NodeId &nodeId) {
    UA_NodeId n = start.get(); // use node ids to browse with
    int level = 0;

    if (path.size() > 0) {
        ServerBrowser b(*this);
        while (level < int(path.size())) {
            b.browse(n);
            auto i = b.find(path[level]);
            if (i == b.list().end())  break;
            level++;
            n = (*i).childId; // shallow copy
        }
        nodeId = n;
        NodeId newNode;
        while (level < int(path.size())) {
            if (!addFolder(nodeId, path[level], NodeId::Null, newNode.notNull(), nameSpaceIndex)) break;
            nodeId = newNode; // assign
            level++;
        }
    }
    return level == int(path.size());
}

bool Server::getChild(NodeId &start,  const std::string &childName, NodeId &ret) {
    Path p;
    p.push_back(childName);
    return nodeIdFromPath(start, p, ret);
}

bool Server::addFolder(
    NodeId& parent,
    const std::string& browseName,
    NodeId& nodeId,
    NodeId& newNode     /*= NodeId::Null*/,
    int nameSpaceIndex  /*= 0*/) {

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

bool Server::addHistoricalVariable(
    NodeId& parent,
    const std::string& broseName,
    Variant& value,
    NodeId& nodeId,
    NodeId& newNode         /*= NodeId::Null*/,
    NodeContext* context    /*= nullptr*/,
    int nameSpaceIndex      /*= 0*/) {

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

bool Server::addProperty(
    NodeId& parent,
    const std::string& key,
    Variant& value,
    NodeId& nodeId          /*= NodeId::Null*/,
    NodeId& newNode         /*= NodeId::Null*/,
    NodeContext* context    /*= nullptr*/,
    int nameSpaceIndex      /*= 0*/) {
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

bool Server::addServerMethod(
    ServerMethod* method,
    const std::string& browseName,
    NodeId& parent,
    NodeId& nodeId,
    NodeId& newNode     /*= NodeId::Null*/,
    int nameSpaceIndex  /*= 0*/) {

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

void Server::serverOnNetworkCallback(const UA_ServerOnNetwork *serverNetwork,
                                    UA_Boolean isServerAnnounce,
                                    UA_Boolean isTxtReceived,
                                    void *data) {
    Server *p = (Server *)(data);
    if (p) p->serverOnNetwork(serverNetwork, isServerAnnounce, isTxtReceived);
}

void Server::registerServerCallback(const UA_RegisteredServer *registeredServer, void *data) {
    if (Server *p = (Server *)(data))
        p->registerServer(registeredServer);
}

bool Server::registerDiscovery(Client &client,  const std::string &semaphoreFilePath) {
    _lastError = UA_Server_register_discovery(_server, client.client(), semaphoreFilePath.empty() ? nullptr : semaphoreFilePath.c_str());
    return lastOK();
}

bool Server::unregisterDiscovery(Client &client) {
    if (!server()) return false;

    _lastError = UA_Server_unregister_discovery(server(), client.client());
    return lastOK();
}

bool   Server::addPeriodicServerRegister(const std::string &discoveryServerUrl, // url must persist - that is be static
                                        Client &client,
                                        UA_UInt64 &periodicCallbackId,
                                        UA_UInt32 intervalMs, // default to 10 minutes
                                        UA_UInt32 delayFirstRegisterMs) {
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
