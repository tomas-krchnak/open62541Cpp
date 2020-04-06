/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#ifndef OPEN62541CLIENT_H
#define OPEN62541CLIENT_H

#include "open62541objects.h"
#include <clientsubscription.h>

/*
    OPC nodes are just data objects they do not need to be in a property tree.
    Nodes can be referred to by name or number (or GUID) which is a hash index to the item in the server
*/


namespace Open62541 {

// Only really for receiving lists. not safe to copy
class UA_EXPORT ApplicationDescriptionList : public std::vector<UA_ApplicationDescription *> {
public:
    ApplicationDescriptionList() {}
    ~ApplicationDescriptionList() {
        for (auto i : *this) {
            if (i) {
                UA_ApplicationDescription_delete(i); // delete the item
            }
        }
    }
};

// dictionary of subscriptions associated with a Client
typedef std::shared_ptr<ClientSubscription> ClientSubscriptionRef;
typedef std::map<UA_UInt32, ClientSubscriptionRef> ClientSubscriptionMap;

/**
 * The Client class
 * This class wraps the corresponding C functions. Refer to the C documentation for a full explanation.
 * The main thing to watch for is Node ID objects are passed by reference. There are stock Node Id objects including NodeId::Null
 * Pass NodeId::Null where a NULL UA_NodeId pointer is expected.
 * If a NodeId is being passed to receive a value use the notNull() method to mark it as a receiver of a new node id.
 * Most functions return true if the lastError is UA_STATUSCODE_GOOD.
*/
class Client {
    UA_Client *_client = nullptr;
    ReadWriteMutex _mutex;
    ClientSubscriptionMap _subscriptions;

protected:
    UA_StatusCode _lastError = 0;

private:
    /**
    * Call Backs
    * @param client
    * @param clientState
    */
    static void  stateCallback(UA_Client *client, UA_ClientState clientState);

    /**
     * asyncConnectCallback
     * @param client
     * @param userdata
     * @param requestId
     * @param response
     */
    static void asyncConnectCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response) {
        Client *p = (Client *)(UA_Client_getContext(client));
        if (p) {
            p->asyncConnectService(requestId, userdata, response);
        }
    }

public:

    // must connect to have a valid client
    Client() : _client(nullptr) {
    }

    /**
     * ~Open62541Client
     */
    virtual ~Client() {
        if (_client)
        {
            disconnect();
            UA_Client_delete(_client);
        }
    }

    /**
     * runIterate
     * @param interval
     * @return 
     */
    bool runIterate(uint32_t interval = 100)
    {
        if(_client)
        {
            _lastError = UA_Client_run_iterate(_client,interval);
            return lastOK();
        }
        return false;
    }

    /**
     * initialise
     */
    void initialise()
    {
        if(_client)
        {
            if(getState() != UA_CLIENTSTATE_DISCONNECTED) disconnect();
            UA_Client_delete(_client);
            _client = nullptr;
        }
        _client = UA_Client_new();
        if (_client) {
            UA_ClientConfig_setDefault(UA_Client_getConfig(_client)); // initalise the client structure
            UA_Client_getConfig(_client)->clientContext = this;
            UA_Client_getConfig(_client)->stateCallback = stateCallback;
            UA_Client_getConfig(_client)->subscriptionInactivityCallback = subscriptionInactivityCallback;
        }
    }

    /**
     * asyncService - handles callbacks when connected async mode
     * @param requestId
     * @param response
     */
    virtual void asyncConnectService(UA_UInt32 /*requestId*/, void* /*userData*/, void* /*response*/) {

    }

    /**
     * getContext
     * @return 
     */
    void *getContext() {
        return UA_Client_getContext(client());
    }

    /**
     * subscriptionInactivityCallback
     * @param client
     * @param subscriptionId
     * @param subContext
     */
    static void subscriptionInactivityCallback(UA_Client *client, UA_UInt32 subscriptionId, void *subContext);

    /**
     * subscriptionInactivity
     * @param subscriptionId
     * @param subContext
     */
    virtual void subscriptionInactivity(UA_UInt32 /*subscriptionId*/, void * /*subContext*/) {}

    /**
     * subscriptions
     * @return map of subscriptions
     */
    ClientSubscriptionMap &subscriptions() {
        return  _subscriptions;
    }
    /**
     * addSubscription
     * @param newId receives Id of created subscription
     * @return true on success
     */
    bool addSubscription(UA_UInt32 &newId, CreateSubscriptionRequest *settings = nullptr) {
        ClientSubscriptionRef c(new ClientSubscription(*this));

        if (settings) {
            c->settings() = *settings; // assign settings across
        }

        if (c->create()) {
            newId = c->id();
            subscriptions()[newId] = c;
            return true;
        }

        return false;
    }

    /**
     * removeSubscription
     * @param Id
     * @return true on success
     */
    bool removeSubscription(UA_UInt32 Id) {
        subscriptions().erase(Id); // remove from dictionary implicit delete
        return true;
    }

    /**
     * subscription
     * @param Id
     * @return pointer to subscription object or null
     */
    ClientSubscription *subscription(UA_UInt32 Id) {
        if (subscriptions().find(Id) != subscriptions().end()) {
            ClientSubscriptionRef &c = subscriptions()[Id];
            return c.get();
        }
        return nullptr;
    }

    // Connection state handlers

    /**
     * stateDisconnected
     */
    virtual void stateDisconnected() {
        OPEN62541_TRC;
    }

    /**
     * stateConnected
     */
    virtual void stateConnected() {
        OPEN62541_TRC;
    }

    /**
     * stateSecureChannel
     */
    virtual void stateSecureChannel() {
        OPEN62541_TRC;
    }

    /**
     * stateSession
     */
    virtual void stateSession() {
        OPEN62541_TRC;
    }

    /**
     * stateSessionRenewed
     */
    virtual void stateSessionRenewed() {
        OPEN62541_TRC;
    }

    /**
     * stateWaitingForAck
     */
    virtual void stateWaitingForAck() {
        OPEN62541_TRC;

    }

    /**
     * stateSessionDisconnected
     */
    virtual void stateSessionDisconnected() {
        OPEN62541_TRC;

    }

    /**
     * stateChange
     * @param clientState
     */
    virtual void stateChange(UA_ClientState clientState) {
        switch (clientState) {
        case UA_CLIENTSTATE_DISCONNECTED:
            stateDisconnected();
            break;
        case UA_CLIENTSTATE_CONNECTED:
            stateConnected();
            break;
        case UA_CLIENTSTATE_SECURECHANNEL:
            stateSecureChannel();
            break;
        case UA_CLIENTSTATE_SESSION:
            stateSession();
            break;
        case UA_CLIENTSTATE_SESSION_RENEWED:
            stateSessionRenewed();
            break;
        case UA_CLIENTSTATE_WAITING_FOR_ACK:
            stateWaitingForAck();
            break;
        case UA_CLIENTSTATE_SESSION_DISCONNECTED:
            stateSessionDisconnected();
            break;
        default:
            break;
        }
    }


    /**
     * Retrieve end points
     * @param serverUrl
     * @param list
     * @return true on success
     */
    bool getEndpoints(const std::string &serverUrl, EndpointDescriptionArray &list) {
        if (!_client) return false;
        WriteLock l(_mutex);
        size_t endpointDescriptionsSize = 0;
        UA_EndpointDescription *endpointDescriptions = nullptr;
        _lastError = UA_Client_getEndpoints(_client, serverUrl.c_str(),
                                            &endpointDescriptionsSize,
                                            &endpointDescriptions);
        if (lastOK()) {
            // copy list so it is managed by the caller
            list.setList(endpointDescriptionsSize, endpointDescriptions);
        }
        return lastOK();
    }

    /**
     * findServers
     * @param serverUrl
     * @param serverUris
     * @param localeIds
     * @param registeredServers
     * @return true on success
     */
    bool findServers(const std::string &serverUrl,
                      StringArray &serverUris,
                      StringArray &localeIds,
                      ApplicationDescriptionArray &registeredServers) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_findServers(_client, serverUrl.c_str(),
                                            serverUris.length(), serverUris.data(),
                                            localeIds.length(), localeIds.data(),
                                            registeredServers.lengthRef(),
                                            registeredServers.dataRef());
        UAPRINTLASTERROR(_lastError)
        return lastOK();
    }

    /**
     * findServersOnNetwork
     * @param serverUrl
     * @param startingRecordId
     * @param maxRecordsToReturn
     * @param serverCapabilityFilter
     * @param serverOnNetwork
     * @return true on success
     */
    bool findServersOnNetwork(const std::string &serverUrl, unsigned startingRecordId,
                              unsigned maxRecordsToReturn, StringArray &serverCapabilityFilter,
                              ServerOnNetworkArray &serverOnNetwork) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError =
            UA_Client_findServersOnNetwork(_client, serverUrl.c_str(),
                                            startingRecordId,  maxRecordsToReturn,
                                            serverCapabilityFilter.length(), serverCapabilityFilter.data(),
                                            serverOnNetwork.lengthRef(), serverOnNetwork.dataRef());
        return lastOK();
    }

    /**
     * readAttribute
     * @param nodeId
     * @param attributeId
     * @param out
     * @param outDataType
     * @return true on success
     */
    bool readAttribute(const UA_NodeId *nodeId,  UA_AttributeId attributeId, void *out, const UA_DataType *outDataType) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = __UA_Client_readAttribute(_client, nodeId, attributeId, out, outDataType);
        return lastOK();
    }

    /**
     * writeAttribute
     * @param nodeId
     * @param attributeId
     * @param in
     * @param inDataType
     * @return true on success
     */
    bool writeAttribute(const UA_NodeId *nodeId, UA_AttributeId attributeId, const void *in,  const UA_DataType *inDataType) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = __UA_Client_writeAttribute(_client, nodeId, attributeId, in, inDataType);
        return lastOK();
    }

    /**
     * mutex
     * @return  client read/write mutex
     */
    ReadWriteMutex &mutex() {
        return _mutex;
    }

    /**
     * getState
     * @return connection state
     */
    UA_ClientState getState() {
        ReadLock l(_mutex);
        if (_client) return UA_Client_getState(_client);
        throw std::runtime_error("Null client");
        return  UA_CLIENTSTATE_DISCONNECTED;
    }

    /**
     * reset
     */
    void reset() {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        UA_Client_reset(_client);
        return;
    }

    /**
     * client
     * @return underlying client object
     */
    UA_Client *client() {
        ReadLock l(_mutex);
        return _client;
    }

    /**
     * config
     * @return client configuration
     */
    UA_ClientConfig &config() {
        return *UA_Client_getConfig(_client);
    }

    /**
     * lastError
     * @return last error set
     */

    UA_StatusCode lastError() {
        return  _lastError;
    }

    // Connect and Disconnect

    /**
     * connect
     * @param endpointUrl
     * @return true on success
     */
    bool connect(const std::string &endpointUrl) {
        initialise();
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        _lastError = UA_Client_connect(_client, endpointUrl.c_str());
        return lastOK();
    }

    /**
     * Connect to the selected server with the given username and password
     * @param client to use
     * @param endpointURL to connect (for example "opc.tcp://localhost:16664")
     * @param username
     * @param password
     * @return Indicates whether the operation succeeded or returns an error code
     */
    bool connectUsername(const std::string &endpoint, const std::string &username, const std::string &password) {
        initialise();
        WriteLock l(_mutex);
        if (!_client)throw std::runtime_error("Null client");
        _lastError = UA_Client_connect_username(_client, endpoint.c_str(), username.c_str(), password.c_str());
        return lastOK();
    }
    /**
     * connectAsync
     * @param endpoint
     * @return 
     */
    bool connectAsync(const std::string &endpoint) {
        initialise();
        WriteLock l(_mutex);
        if (!_client)throw std::runtime_error("Null client");
        _lastError = UA_Client_connect_async(_client, endpoint.c_str(), asyncConnectCallback, this);
        return lastOK();
    }

    /**
     * Connect to the server without creating a session
     * @param client to use
     * @param endpointURL to connect (for example "opc.tcp://localhost:4840")
     * @return Indicates whether the operation succeeded or returns an error code
     */
    bool connectNoSession(const std::string &endpoint) {
        initialise();
        WriteLock l(_mutex);
        if (!_client)throw std::runtime_error("Null client");
        _lastError =  UA_Client_connect_noSession(_client, endpoint.c_str());
        return lastOK();
    }

    /**
     * disconnect
     * @return 
     */
    bool disconnect() {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        _lastError = UA_Client_disconnect(_client);
        return lastOK();
    }
    /**
     * disconnectAsync
     * @return true on success
     */
    bool disconnectAsync(UA_UInt32 requestId = 0) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        _lastError = UA_Client_disconnect_async(_client, &requestId);
        return lastOK();
    }

    /**
     * manuallyRenewSecureChannel
     * @return 
     */
    bool manuallyRenewSecureChannel() {
        return false;
    }

    /**
     * Gets a list of endpoints of a server.
     * only use for getting string names of end points.
     * @param client to use. Must be connected to the same endpoint given in
     *        serverUrl or otherwise in disconnected state.
     * @param serverUrl url to connect (for example "opc.tcp://localhost:16664")
     * @param endpointDescriptionsSize size of the array of endpoint descriptions
     * @param endpointDescriptions array of endpoint descriptions that is allocated
     *        by the function (you need to free manually)
     * @return whether the operation succeeded or returns an error code
     */
    UA_StatusCode getEndpoints(const std::string &serverUrl, std::vector<std::string> &list);

    /**
     * Get the namespace-index of a namespace-URI.
     * @param client The UA_Client struct for this connection
     * @param namespaceUri The interested namespace URI
     * @param namespaceIndex The namespace index of the URI. The value is unchanged
     *         in case of an error
     * @return Indicates whether the operation succeeded or returns an error code */
    int  namespaceGetIndex(const std::string &namespaceUri) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        int namespaceIndex = 0;
        UA_String s = toUA_String(namespaceUri);
        if (UA_Client_NamespaceGetIndex(_client, &s, (UA_UInt16 *)(&namespaceIndex)) == UA_STATUSCODE_GOOD) {
            return namespaceIndex;
        }
        return -1; // value
    }

    /**
     * browseName
     * @param nodeId
     * @return true on success
     */
    bool  browseName(NodeId &nodeId, std::string &s, int &ns) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        QualifiedName outBrowseName;
        if ((_lastError = UA_Client_readBrowseNameAttribute(_client, nodeId, outBrowseName)) == UA_STATUSCODE_GOOD) {
            s =   toString(outBrowseName.get().name);
            ns = outBrowseName.get().namespaceIndex;
        }
        return _lastError == UA_STATUSCODE_GOOD;
    }

    /**
     * setBrowseName
     * @param nodeId
     * @param nameSpaceIndex
     * @param name
     */
    void setBrowseName(NodeId &nodeId, int nameSpaceIndex, const std::string &name) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        QualifiedName newBrowseName(nameSpaceIndex, name);
        UA_Client_writeBrowseNameAttribute(_client, nodeId, newBrowseName);
    }

    /**
     * browseTree
     * @param nodeId
     * @param node
     * @return true on success
     */
    bool browseTree(UA_NodeId &nodeId, Open62541::UANode *node);

    /**
     * browseTree
     * @param nodeId
     * @return true on success
     */
    bool browseTree(NodeId &nodeId, UANodeTree &tree);

    /**
     * browseTree
     * @param nodeId
     * @param tree
     * @return 
     */
    bool browseTree(NodeId &nodeId, UANode *tree);

    /**
     * browse and create a map of string ids to node ids
     * @param nodeId
     * @param tree
     * @return 
     */
    bool browseTree(NodeId &nodeId, NodeIdMap &m); 

    /**
     * browseChildren
     * @param nodeId
     * @param m
     * @return  true on success
     */
    bool browseChildren(UA_NodeId &nodeId, NodeIdMap &m);

    /**
     * NodeIdFromPath get the node id from the path of browse names in the given namespace. Tests for node existence
     * @param path
     * @param nodeId
     * @return  true on success
     */
    bool nodeIdFromPath(NodeId &start, Path &path,  NodeId &nodeId);

    /**
     * createPath
     * @param start
     * @param path
     * @param nameSpaceIndex
     * @param nodeId
     * @return  true on success
     */
    bool createFolderPath(NodeId &start, Path &path, int nameSpaceIndex, NodeId &nodeId);

    /**
     * getChild
     * @param nameSpaceIndex
     * @param childName
     * @return 
     */
    bool getChild(NodeId &start,  const std::string &childName, NodeId &ret);

    /**
     * addFolder
     * @param parent
     * @param nameSpaceIndex
     * @param childName
     * @return  true on success
     */
    bool addFolder(NodeId &parent,  const std::string &childName,
                    NodeId &nodeId, NodeId &newNode = NodeId::Null, int nameSpaceIndex = 0);

    /**
     * addVariable
     * @param parent
     * @param nameSpaceIndex
     * @param childName
     * @return  true on success
     */
    bool addVariable(NodeId &parent, const std::string &childName, Variant &value,
                      NodeId &nodeId, NodeId &newNode = NodeId::Null, int nameSpaceIndex = 0);

    /**
     * setVariable
     * @param nodeId
     * @param value
     * @return  true on success
     */
    bool  setVariable(NodeId &nodeId,  Variant &value) {
        if (!_client) return false;
        _lastError = UA_Client_writeValueAttribute(_client,  nodeId, value);
        return lastOK();
    }

    // Attribute access generated from the docs

    /**
     * readNodeIdAttribute
     * @param nodeId
     * @param outNodeId
     * @return  true on success
     */
    bool
    readNodeIdAttribute(NodeId &nodeId,
                        UA_NodeId &outNodeId) {
        return   readAttribute(nodeId, UA_ATTRIBUTEID_NODEID,
                                &outNodeId, &UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * readNodeClassAttribute
     * @param nodeId
     * @param outNodeClass
     * @return  true on success
     */
    bool
    readNodeClassAttribute(NodeId &nodeId,
                            UA_NodeClass &outNodeClass) {
        return   readAttribute(nodeId, UA_ATTRIBUTEID_NODECLASS,
                                &outNodeClass, &UA_TYPES[UA_TYPES_NODECLASS]);
    }

    /**
     * readBrowseNameAttribute
     * @param nodeId
     * @param outBrowseName
     * @return  true on success
     */
    bool
    readBrowseNameAttribute(NodeId &nodeId,
                            QualifiedName &outBrowseName) {
        return   readAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME,
                                outBrowseName,
                                &UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
    }

    /**
     * readDisplayNameAttribute
     * @param nodeId
     * @param outDisplayName
     * @return  true on success
     */
    bool
    readDisplayNameAttribute(NodeId &nodeId,
                              LocalizedText &outDisplayName) {
        return   readAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME,
                                outDisplayName,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    }

    /**
     * readDescriptionAttribute
     * @param nodeId
     * @param outDescription
     * @return  true on success
     */
    bool
    readDescriptionAttribute(NodeId &nodeId,
                              LocalizedText &outDescription) {
        return   readAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION,
                                outDescription,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
        return lastOK();
    }

    /**
     * readWriteMaskAttribute
     * @param nodeId
     * @param outWriteMask
     * @return  true on success
     */
    bool
    readWriteMaskAttribute(NodeId &nodeId,
                            UA_UInt32 &outWriteMask) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK,
                              &outWriteMask, &UA_TYPES[UA_TYPES_UINT32]);

    }

    /**
     * readUserWriteMaskAttribute
     * @param nodeId
     * @param outUserWriteMask
     * @return  true on success
     */
    bool
    readUserWriteMaskAttribute(NodeId &nodeId,
                                UA_UInt32 &outUserWriteMask) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_USERWRITEMASK,
                              &outUserWriteMask,
                              &UA_TYPES[UA_TYPES_UINT32]);

    }

    /**
     * readIsAbstractAttribute
     * @param nodeId
     * @param outIsAbstract
     * @return  true on success
     */
    bool
    readIsAbstractAttribute(NodeId &nodeId,
                            UA_Boolean &outIsAbstract) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT,
                              &outIsAbstract, &UA_TYPES[UA_TYPES_BOOLEAN]);

    }

    /**
     * readSymmetricAttribute
     * @param nodeId
     * @param outSymmetric
     * @return  true on success
     */
    bool
    readSymmetricAttribute(NodeId &nodeId,
                            UA_Boolean &outSymmetric) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_SYMMETRIC,
                              &outSymmetric, &UA_TYPES[UA_TYPES_BOOLEAN]);

    }

    /**
     * readInverseNameAttribute
     * @param nodeId
     * @param outInverseName
     * @return  true on success
     */
    bool
    readInverseNameAttribute(NodeId &nodeId,
                              LocalizedText &outInverseName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME,
                              outInverseName,
                              &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    }

    /**
     * readContainsNoLoopsAttribute
     * @param nodeId
     * @param outContainsNoLoops
     * @return  true on success
     */
    bool
    readContainsNoLoopsAttribute(NodeId &nodeId,
                                  UA_Boolean &outContainsNoLoops) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_CONTAINSNOLOOPS,
                              &outContainsNoLoops,
                              &UA_TYPES[UA_TYPES_BOOLEAN]);

    }

    /**
     * readEventNotifierAttribute
     * @param nodeId
     * @param outEventNotifier
     * @return  true on success
     */
    bool
    readEventNotifierAttribute(NodeId &nodeId,
                                UA_Byte &outEventNotifier) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EVENTNOTIFIER,
                              &outEventNotifier, &UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * readValueAttribute
     * @param nodeId
     * @param outValue
     * @return  true on success
     */
    bool
    readValueAttribute(NodeId &nodeId,
                        Variant &outValue) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                              outValue, &UA_TYPES[UA_TYPES_VARIANT]);
    }

    /**
     * readDataTypeAttribute
     * @param nodeId
     * @param outDataType
     * @return  true on success
     */
    bool
    readDataTypeAttribute(NodeId &nodeId,
                          UA_NodeId &outDataType) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE,
                              &outDataType, &UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * readValueRankAttribute
     * @param nodeId
     * @param outValueRank
     * @return  true on success
     */
    bool
    readValueRankAttribute(NodeId &nodeId,
                            UA_Int32 &outValueRank) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK,
                              &outValueRank, &UA_TYPES[UA_TYPES_INT32]);
    }

    /**
     * readArrayDimensionsAttribute
     * @param nodeId
     * @param ret
     * @return true on success
     */
    bool readArrayDimensionsAttribute(NodeId &nodeId, std::vector<UA_UInt32> &ret) {
        if (!_client) return false;
        WriteLock l(_mutex);
        size_t outArrayDimensionsSize;
        UA_UInt32 *outArrayDimensions = nullptr;
        _lastError = UA_Client_readArrayDimensionsAttribute(_client, nodeId, &outArrayDimensionsSize, &outArrayDimensions);
        if (_lastError == UA_STATUSCODE_GOOD) {
            if (outArrayDimensions) {
                for (int i = 0; i < int(outArrayDimensionsSize); i++) {
                    ret.push_back(outArrayDimensions[i]);
                }
                UA_Array_delete(outArrayDimensions, outArrayDimensionsSize, &UA_TYPES[UA_TYPES_INT32]);
            }
        }
        return lastOK();
    }

    /**
     * readAccessLevelAttribute
     * @param nodeId
     * @param outAccessLevel
     * @return  true on success
     */
    bool
    readAccessLevelAttribute(NodeId &nodeId,
                              UA_Byte &outAccessLevel) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL,
                              &outAccessLevel, &UA_TYPES[UA_TYPES_BYTE]);

    }

    /**
     * readUserAccessLevelAttribute
     * @param nodeId
     * @param outUserAccessLevel
     * @return  true on success
     */
    bool
    readUserAccessLevelAttribute(NodeId &nodeId,
                                  UA_Byte &outUserAccessLevel) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_USERACCESSLEVEL,
                              &outUserAccessLevel,
                              &UA_TYPES[UA_TYPES_BYTE]);

    }

    /**
     * readMinimumSamplingIntervalAttribute
     * @param nodeId
     * @param outMinSamplingInterval
     * @return  true on success
     */
    bool
    readMinimumSamplingIntervalAttribute(NodeId &nodeId,
                                          UA_Double &outMinSamplingInterval) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                              &outMinSamplingInterval,
                              &UA_TYPES[UA_TYPES_DOUBLE]);

    }

    /**
     * readHistorizingAttribute
     * @param nodeId
     * @param outHistorizing
     * @return  true on success
     */
    bool
    readHistorizingAttribute(NodeId &nodeId,
                              UA_Boolean &outHistorizing) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_HISTORIZING,
                              &outHistorizing, &UA_TYPES[UA_TYPES_BOOLEAN]);

    }

    /**
     * readExecutableAttribute
     * @param nodeId
     * @param outExecutable
     * @return  true on success
     */
    bool
    readExecutableAttribute(NodeId &nodeId,
                            UA_Boolean &outExecutable) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE,
                              &outExecutable, &UA_TYPES[UA_TYPES_BOOLEAN]);

    }

    /**
     * readUserExecutableAttribute
     * @param nodeId
     * @param outUserExecutable
     * @return  true on success
     */
    bool
    readUserExecutableAttribute(NodeId &nodeId,
                                UA_Boolean &outUserExecutable) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_USEREXECUTABLE,
                              &outUserExecutable,
                              &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * setNodeIdAttribute
     * @param nodeId
     * @param newNodeId
     * @return  true on success
     */
    bool
    setNodeIdAttribute(NodeId &nodeId,
                        NodeId &newNodeId) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_NODEID,
                                &newNodeId, &UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * setNodeClassAttribute
     * @param nodeId
     * @param newNodeClass
     * @return  true on success
     */
    bool
    setNodeClassAttribute(NodeId &nodeId,
                          UA_NodeClass &newNodeClass) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_NODECLASS,
                                &newNodeClass, &UA_TYPES[UA_TYPES_NODECLASS]);
    }

    /**
     * setBrowseNameAttribute
     * @param nodeId
     * @param newBrowseName
     * @return  true on success
     */
    bool
    setBrowseNameAttribute(NodeId &nodeId,
                            QualifiedName &newBrowseName) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME,
                                &newBrowseName,
                                &UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
    }

    /**
     * setDisplayNameAttribute
     * @param nodeId
     * @param newDisplayName
     * @return  true on success
     */
    bool
    setDisplayNameAttribute(NodeId &nodeId,
                            LocalizedText &newDisplayName) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME,
                                &newDisplayName,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * setDescriptionAttribute
     * @param nodeId
     * @param newDescription
     * @return  true on success
     */
    bool
    setDescriptionAttribute(NodeId &nodeId,
                            LocalizedText &newDescription) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION,
                                newDescription,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * setWriteMaskAttribute
     * @param nodeId
     * @param newWriteMask
     * @return  true on success
     */
    bool
    setWriteMaskAttribute(NodeId &nodeId,
                          UA_UInt32 newWriteMask) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK,
                                &newWriteMask, &UA_TYPES[UA_TYPES_UINT32]);
    }

    /**
     * setUserWriteMaskAttribute
     * @param nodeId
     * @param newUserWriteMask
     * @return  true on success
     */
    bool
    setUserWriteMaskAttribute(NodeId &nodeId,
                              UA_UInt32 newUserWriteMask) {
        return   writeAttribute(nodeId,
                                UA_ATTRIBUTEID_USERWRITEMASK,
                                &newUserWriteMask,
                                &UA_TYPES[UA_TYPES_UINT32]);
    }

    /**
     * setIsAbstractAttribute
     * @param nodeId
     * @param newIsAbstract
     * @return  true on success
     */
    bool
    setIsAbstractAttribute(NodeId &nodeId,
                            UA_Boolean newIsAbstract) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT,
                                &newIsAbstract, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * setSymmetricAttribute
     * @param nodeId
     * @param newSymmetric
     * @return  true on success
     */
    bool
    setSymmetricAttribute(NodeId &nodeId,
                          UA_Boolean newSymmetric) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_SYMMETRIC,
                                &newSymmetric, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * setInverseNameAttribute
     * @param nodeId
     * @param newInverseName
     * @return  true on success
     */
    bool
    setInverseNameAttribute(NodeId &nodeId,
                            LocalizedText &newInverseName) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME,
                                &newInverseName,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * setContainsNoLoopsAttribute
     * @param nodeId
     * @param newContainsNoLoops
     * @return  true on success
     */
    bool
    setContainsNoLoopsAttribute(NodeId &nodeId,
                                UA_Boolean &newContainsNoLoops) {
        return   writeAttribute(nodeId,
                                UA_ATTRIBUTEID_CONTAINSNOLOOPS,
                                &newContainsNoLoops,
                                &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * setEventNotifierAttribute
     * @param nodeId
     * @param newEventNotifier
     * @return  true on success
     */
    bool
    setEventNotifierAttribute(NodeId &nodeId,
                              UA_Byte newEventNotifier) {
        return   writeAttribute(nodeId,
                                UA_ATTRIBUTEID_EVENTNOTIFIER,
                                &newEventNotifier,
                                &UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * setValueAttribute
     * @param nodeId
     * @param newValue
     * @return  true on success
     */
    bool
    setValueAttribute(NodeId &nodeId,
                      Variant &newValue) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                                newValue, &UA_TYPES[UA_TYPES_VARIANT]);
    }

    /**
     * setDataTypeAttribute
     * @param nodeId
     * @param newDataType
     * @return  true on success
     */
    bool
    setDataTypeAttribute(NodeId &nodeId,
                          const UA_NodeId *newDataType) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE,
                                newDataType, &UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * setValueRankAttribute
     * @param nodeId
     * @param newValueRank
     * @return   true on success
     */
    bool
    setValueRankAttribute(NodeId &nodeId,
                          UA_Int32 newValueRank) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK,
                                &newValueRank, &UA_TYPES[UA_TYPES_INT32]);
    }

    /**
     * setArrayDimensionsAttribute
     * @param nodeId
     * @param newArrayDimensions
     * @return   true on success
     */
    bool
    setArrayDimensionsAttribute(NodeId &nodeId,
                                std::vector<UA_UInt32> &newArrayDimensions) {
        UA_UInt32 v = newArrayDimensions.size();
        _lastError = UA_Client_writeArrayDimensionsAttribute(_client, nodeId, v,
                                                              newArrayDimensions.data());
        return lastOK();
    }

    /**
     * setAccessLevelAttribute
     * @param nodeId
     * @param newAccessLevel
     * @return   true on success
     */
    bool
    setAccessLevelAttribute(NodeId &nodeId,
                            UA_Byte newAccessLevel) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL,
                                &newAccessLevel, &UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * setUserAccessLevelAttribute
     * @param nodeId
     * @param newUserAccessLevel
     * @return   true on success
     */
    bool
    setUserAccessLevelAttribute(NodeId &nodeId,
                                UA_Byte newUserAccessLevel) {
        return   writeAttribute(nodeId,
                                UA_ATTRIBUTEID_USERACCESSLEVEL,
                                &newUserAccessLevel,
                                &UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * setMinimumSamplingIntervalAttribute
     * @param nodeId
     * @param newMinInterval
     * @return   true on success
     */
    bool
    setMinimumSamplingIntervalAttribute(
        NodeId &nodeId,
        UA_Double newMinInterval) {
        return   writeAttribute(nodeId,
                                UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                                &newMinInterval, &UA_TYPES[UA_TYPES_DOUBLE]);
    }

    /**
     * setHistorizingAttribute
     * @param nodeId
     * @param newHistorizing
     * @return   true on success
     */
    bool
    setHistorizingAttribute(NodeId &nodeId,
                            UA_Boolean newHistorizing) {
        return   writeAttribute(nodeId, UA_ATTRIBUTEID_HISTORIZING,
                                &newHistorizing, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * setExecutableAttribute
     * @param nodeId
     * @param newExecutable
     * @return   true on success
     */
    bool
    setExecutableAttribute(NodeId &nodeId,
                            UA_Boolean newExecutable) {
        _lastError =  writeAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE,
                                      &newExecutable, &UA_TYPES[UA_TYPES_BOOLEAN]);
        return lastOK();
    }

    /**
     * setUserExecutableAttribute
     * @param nodeId
     * @param newUserExecutable
     * @return   true on success
     */
    bool
    setUserExecutableAttribute(NodeId &nodeId,
                                UA_Boolean newUserExecutable) {
        _lastError =  writeAttribute(nodeId,
                                      UA_ATTRIBUTEID_USEREXECUTABLE,
                                      &newUserExecutable,
                                      &UA_TYPES[UA_TYPES_BOOLEAN]);
        return lastOK();
    }

    // End of attributes

    /**
     * variable
     * @param nodeId
     * @param value
     * @return   true on success
     */
    bool  variable(NodeId &nodeId,  Variant &value) {
        if (!_client) return false;
        WriteLock l(_mutex);
        // outValue is managed by caller - transfer to output value
        value.clear();
        _lastError = UA_Client_readValueAttribute(_client, nodeId, value); // shallow copy
        return lastOK();
    }

    /**
     * nodeClass
     * @param nodeId
     * @param c
     * @return   true on success
     */
    bool nodeClass(NodeId &nodeId, NodeClass &c) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        _lastError = UA_Client_readNodeClassAttribute(_client, nodeId, &c);
        return lastOK();
    }

    /**
     * deleteNode
     * @param nodeId
     * @param deleteReferences
     * @return   true on success
     */
    bool deleteNode(NodeId &nodeId, bool  deleteReferences) {
        WriteLock l(_mutex);
        if (!_client) throw std::runtime_error("Null client");
        _lastError =  UA_Client_deleteNode(_client, nodeId, UA_Boolean(deleteReferences));
        return lastOK();
    }

    /**
     * deleteTree
     * @param nodeId
     * @return   true on success
     */
    bool deleteTree(NodeId &nodeId); // recursive delete

    /**
     * Client::deleteChildren
     * @param n
     */
    void deleteChildren(UA_NodeId &n);

    /**
     * callMethod
     * @param objectId
     * @param methodId
     * @param in
     * @param out
     * @return   true on success
     */
    bool callMethod(NodeId &objectId,  NodeId &methodId, VariantList &in, VariantCallResult &out) {
        WriteLock l(_mutex);
        size_t outputSize = 0;
        UA_Variant *output = nullptr;
        if (!_client) throw std::runtime_error("Null client");
        _lastError = UA_STATUSCODE_GOOD;
        _lastError = UA_Client_call(_client,  objectId,
                                    methodId, in.size(), in.data(),
                                    &outputSize, &output);
        if (_lastError == UA_STATUSCODE_GOOD) {
            out.set(output, outputSize);
        }
        return lastOK();
    }

    /**
     * process
     * @return   true on success
     */
    virtual bool process() {
        return true;
    }

    /**
     * lastOK
     * @return   true if last error is UA_STATUSCODE_GOOD
     */
    bool lastOK() const {
        return _lastError == UA_STATUSCODE_GOOD;
    }

    // Add nodes - templated from docs

    /**
     * addVariableTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addVariableTypeNode(
        NodeId  &requestedNewNodeId,
        NodeId  &parentNodeId,
        NodeId  &referenceTypeId,
        QualifiedName &browseName,
        VariableTypeAttributes &attr,
        NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addVariableTypeNode(_client,
                                                    requestedNewNodeId,
                                                    parentNodeId,
                                                    referenceTypeId,
                                                    browseName,
                                                    attr,
                                                    outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();
    }

    /**
     * addObjectNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param typeDefinition
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addObjectNode(NodeId  &requestedNewNodeId,
                  NodeId  &parentNodeId,
                  NodeId  &referenceTypeId,
                  QualifiedName &browseName,
                  NodeId  &typeDefinition,
                  ObjectAttributes &attr,
                  NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addObjectNode(_client,
                                              requestedNewNodeId,
                                              parentNodeId,
                                              referenceTypeId,
                                              browseName,
                                              typeDefinition,
                                              attr,
                                              outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();

    }

    /**
     * addObjectTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addObjectTypeNode(NodeId  &requestedNewNodeId,
                      NodeId  &parentNodeId,
                      NodeId  &referenceTypeId,
                      QualifiedName &browseName,
                      ObjectTypeAttributes &attr,
                      NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addObjectTypeNode(_client,
                                                  requestedNewNodeId,
                                                  parentNodeId,
                                                  referenceTypeId,
                                                  browseName,
                                                  attr,
                                                  outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();
    }

    /**
     * addViewNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addViewNode(NodeId  &requestedNewNodeId,
                NodeId  &parentNodeId,
                NodeId  &referenceTypeId,
                QualifiedName &browseName,
                ViewAttributes &attr,
                NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addViewNode(_client,
                                            requestedNewNodeId,
                                            parentNodeId,
                                            referenceTypeId,
                                            browseName,
                                            attr,
                                            outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();

    }

    /**
     * addReferenceTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addReferenceTypeNode(
        NodeId  &requestedNewNodeId,
        NodeId  &parentNodeId,
        NodeId  &referenceTypeId,
        QualifiedName &browseName,
        ReferenceTypeAttributes &attr,
        NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addReferenceTypeNode(_client,
                                                    requestedNewNodeId,
                                                    parentNodeId,
                                                    referenceTypeId,
                                                    browseName,
                                                    attr,
                                                    outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();

    }

    /**
     * addDataTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addDataTypeNode(NodeId  &requestedNewNodeId,
                    NodeId  &parentNodeId,
                    NodeId  &referenceTypeId,
                    QualifiedName &browseName,
                    DataTypeAttributes &attr,
                    NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addDataTypeNode(_client,
                                                requestedNewNodeId,
                                                parentNodeId,
                                                referenceTypeId,
                                                browseName,
                                                attr,
                                                outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();
    }

    /**
     * addMethodNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @return true on success
     */
    bool
    addMethodNode(NodeId  &requestedNewNodeId,
                  NodeId  &parentNodeId,
                  NodeId  &referenceTypeId,
                  QualifiedName &browseName,
                  MethodAttributes &attr,
                  NodeId &outNewNodeId = NodeId::Null) {
        if (!_client) return false;
        WriteLock l(_mutex);
        _lastError = UA_Client_addMethodNode(_client,
                                              requestedNewNodeId,
                                              parentNodeId,
                                              referenceTypeId,
                                              browseName,
                                              attr,
                                              outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();
    }

    /**
     * addProperty
     * @param parent
     * @param key
     * @param value
     * @param nodeId
     * @param newNode
     * @return true on success
     */
    bool addProperty(NodeId &parent,
                      const std::string &key,
                      Variant &value,
                      NodeId &nodeId,
                      NodeId &newNode = NodeId::Null, int nameSpaceIndex = 0);

    // Async services

    /**
     * asyncServiceCallback
     * @param client
     * @param userdata
     * @param requestId
     * @param response
     * @param responseType
     */
    static void asyncServiceCallback(UA_Client *client, void *userdata,
                                      UA_UInt32 requestId, void *response,
                                      const UA_DataType *responseType);

    /**
     * asyncService
     * @param userdata
     * @param requestId
     * @param response
     * @param responseType
     */
    virtual void asyncService(void * /*userdata*/, UA_UInt32 /*requestId*/, void * /*response*/,
                              const UA_DataType * /*responseType*/) {}
    /**
     * historicalIterator
     * @return 
     */
    virtual bool historicalIterator(const NodeId &/*node*/, UA_Boolean /*moreDataAvailable*/,const UA_ExtensionObject &/*data*/) {
        return false;
    }

    /**
     * historicalIteratorCallback
     * @param client
     * @param nodeId
     * @param moreDataAvailable
     * @param data
     * @param callbackContext
     * @return 
     */
    static UA_Boolean historicalIteratorCallback(UA_Client *client, const UA_NodeId *nodeId,   UA_Boolean moreDataAvailable,
                                                  const UA_ExtensionObject *data, void *callbackContext) {
        if (callbackContext && nodeId && data) {
            Client *p = (Client *)callbackContext;
            NodeId n(*nodeId);
            return (p->historicalIterator(n, moreDataAvailable,*data)) ? UA_TRUE : UA_FALSE;
        }
        return UA_FALSE;
    }

    /**
     * historyReadRaw
     * @param n
     * @param startTime
     * @param endTime
     * @param numValuesPerNode
     * @param indexRange
     * @param returnBounds
     * @param timestampsToReturn
     * @return 
     */
    bool  historyReadRaw(const NodeId &n, UA_DateTime startTime, UA_DateTime endTime,
                          unsigned numValuesPerNode, const UA_String &indexRange = UA_STRING_NULL, bool returnBounds = false,
                          UA_TimestampsToReturn timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH) {
        _lastError = UA_Client_HistoryRead_raw(_client, n.constRef(), historicalIteratorCallback,startTime, endTime,
                                                indexRange, returnBounds ? UA_TRUE : UA_FALSE, (UA_UInt32) numValuesPerNode,
                                                timestampsToReturn, this);
        return lastOK();
    }

    /**
     * historyUpdateInsert
     * @param n
     * @param value
     * @return 
     */
    bool historyUpdateInsert(const NodeId &n, const UA_DataValue &value)  {

        _lastError =   UA_Client_HistoryUpdate_insert(_client, n.constRef(), const_cast<UA_DataValue *>(&value));
        return lastOK();
    }

    /**
     * historyUpdateReplace
     * @param n
     * @param value
     * @return 
     */
    bool historyUpdateReplace(const NodeId &n, const UA_DataValue &value) {

        _lastError = UA_Client_HistoryUpdate_replace(_client, n.constRef(), const_cast<UA_DataValue *>(&value));
        return lastOK();
    }

    /**
     * historyUpdateUpdate
     * @param n
     * @param value
     * @return 
     */
    bool historyUpdateUpdate(const NodeId &n, const UA_DataValue &value) {

        _lastError = UA_Client_HistoryUpdate_update(_client, n.constRef(), const_cast<UA_DataValue *>(&value));
        return lastOK();
    }

    /**
     * historyUpdateDeleteRaw
     * @param n
     * @param startTimestamp
     * @param endTimestamp
     * @return 
     */
    bool historyUpdateDeleteRaw(const NodeId &n, UA_DateTime startTimestamp, UA_DateTime endTimestamp) {
        _lastError = UA_Client_HistoryUpdate_deleteRaw(_client, n.constRef(), startTimestamp, endTimestamp);
        return lastOK();
    }

};

} // namespace Open62541

#endif // OPEN62541CLIENT_H
