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

#ifndef CLIENTSUBSCRIPTION_H
#include "clientsubscription.h"
#endif

/*
    OPC nodes are just data objects they do not need to be in a property tree.
    Nodes can be referred to by name or number (or GUID) which is a hash index to the item in the server
*/


namespace Open62541 {

// Only really for receiving lists. not safe to copy
class UA_EXPORT ApplicationDescriptionList : public std::vector<UA_ApplicationDescription *> {
public:
    ApplicationDescriptionList()    {}
    ~ApplicationDescriptionList();
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
    UA_Client*              m_pClient = nullptr;  /**< Underlying UA struct. */
    mutable ReadWriteMutex  m_mutex;
    ClientSubscriptionMap   m_subscriptions;      /**< Map of subscription of the client. */

protected:
    UA_StatusCode           m_lastError = 0;

private:
    /**
     * Call-back called when the client state changes.
     * @param client specify the client
     * @param clientState specify the new state.
     */
    static void stateCallback(UA_Client* client, UA_ClientState clientState);

    /**
     * Call-back called when the asynchronous connection succeed.
     * @param client specify the client
     * @param userdata any data needed by the Call-back.
     * @param requestId id of the connection request.
     * @param response result structure returned by the call-back.
     */
    static void asyncConnectCallback(
        UA_Client*  client,
        void*       userdata,
        UA_UInt32   requestId,
        void*       response);

public:
    /**
     * Constructor. Must be initialized before use.
     */
    Client() = default;

    /**
     * Destructor. Disconnect the UA_client before deleting it.
     */
    virtual ~Client();

    /**
     * Listen on the network and process arriving asynchronous
     * responses in the background.
     * Internal housekeeping, renewal of SecureChannels and subscription
     * management is done as well.
     * @param interval specify the listening cycle duration. If 0... ?
     * @return true on success.
     */
    bool runIterate(uint32_t interval = 100);

    /**
     * initialise the UA_client.
     */
    void initialise();

    /**
     * Hook customizing the asyncConnectCallback call-back.
     * asyncService - handles callbacks when connected in async mode
     * @param requestId
     * @param response
     */
    virtual void asyncConnectService(
        UA_UInt32   requestId,
        void*       userData,
        void*       response) {}

    /**
    * Get a pointer on the UA_Client.
    * @return underlying client object
    * @Warning Not tread-safe.
    */
    UA_Client* client() {
        ReadLock l(m_mutex); // only protect the pointer copy, not its usage.
        return m_pClient;
    }

    /**
    * Return the client configuration. Assumes the client isn't null.
    * @return client configuration
    * @Warning UB if _client is null. Not tread-safe.
    */
    UA_ClientConfig& config()         { return* UA_Client_getConfig(m_pClient); }

    /**
    * Test if the last UA function succeeded.
    * @return true if last error is UA_STATUSCODE_GOOD
    */
    bool lastOK()               const { return m_lastError == UA_STATUSCODE_GOOD; }

    /**
    * Return the error code of the last UA function executed.
    * @return last status code.
    */
    UA_StatusCode lastError()   const { return m_lastError; }

    /**
     * Get the client context. Assumes the client isn't null.
     * @return a pointer on the client context.
     */
    void* getContext()                { return UA_Client_getContext(m_pClient); }

    /**
    * Get the read/write mutex.
    * @return a reference on the client read/write mutex
    */
    ReadWriteMutex& mutex()           { return m_mutex; }

    /**
    * Get the client subscriptions map.
    * @return a reference to the map of subscriptions
    */
    ClientSubscriptionMap& subscriptions() { return m_subscriptions; }

    /**
     * Call-back called when a client subscription timeout is reached.
     * Happens if the PublishResponse isn't received after the defined delay
     * of ``(publishingInterval * maxKeepAliveCount) + timeout)``.
     * @param client specify the client owning the subscription.
     * @param subscriptionId specify the id of the inactive subscription
     * @param subContext specify the subscription context.
     */
    static void subscriptionInactivityCallback(
        UA_Client*  client,
        UA_UInt32   subscriptionId,
        void*       subContext);

    /**
     * Hook customizing the subscriptionInactivityCallback call-back.
     * @param subscriptionId specify the id of the inactive subscription
     * @param subContext specify the subscription context.
     */
    virtual void subscriptionInactivity(UA_UInt32 subscriptionId, void* subContext) {}

    /**
     * Create a new subscription and add it to the subscription map.
     * @param[out] newId receives Id of the created subscription.
     * @param[in] settings optionally specify the subscription settings. If null, default setting are used.
     * @return true on success.
     */
    bool addSubscription(
        UA_UInt32&                  newId,
        CreateSubscriptionRequest*  settings = nullptr);

    /**
     * Remove a subscription from the map, destroying it.
     * @param Id of the subscription to delete.
     * @return true on success.
     */
    bool removeSubscription(UA_UInt32 Id);

    /**
     * Find a subscription by its id.
     * @param Id of the subscription to find
     * @return raw pointer to the found subscription or nullptr.
     */
    ClientSubscription* subscription(UA_UInt32 Id);

    // Connection state handlers

    /**
     * Hook called when the client has disconnected from the server.
     */
    virtual void stateDisconnected() { OPEN62541_TRC; }

    /**
     * Hook called when the client has connected to the server.
     */
    virtual void stateConnected() { OPEN62541_TRC; }

    /**
     * Hook called when the client opens a Secure Channel open.
     */
    virtual void stateSecureChannel() { OPEN62541_TRC; }

    /**
     * Hook called when the client starts a new Session.
     */
    virtual void stateSession() { OPEN62541_TRC; }

    /**
     * Hook called when the client renew its current Session.
     */
    virtual void stateSessionRenewed() { OPEN62541_TRC; }

    /**
     * Hook called when the client state start to wait for Acknowledge.
     */
    virtual void stateWaitingForAck() { OPEN62541_TRC; }

    /**
     * Hook called when the client session ends.
     */
    virtual void stateSessionDisconnected() { OPEN62541_TRC; }

    /**
     * Hook customizing stateCallback call-back, called when the client state changes.
     * @param clientState the new client state that triggered the call-back.
     */
    virtual void stateChange(UA_ClientState clientState);

    /**
     * Gets a server endpoint list in an Array.
     * The client must be connected to the same endpoint given in
     * serverUrl or otherwise in disconnected state.
     * @param serverUrl url to connect (for example "opc.tcp://localhost:4840")
     * @param[out] list array of endpoint descriptions.
     * @return true on success.
     */
    bool getEndpoints(
        const std::string&          serverUrl,
        EndpointDescriptionArray&   list);

    /**
     * Gets a server endpoint list in a vector.
     * The client must be connected to the same endpoint given in
     * serverUrl or otherwise in disconnected state.
     * @param serverUrl url to connect (for example "opc.tcp://localhost:4840")
     * @param[out] list vector of endpoint descriptions.
     * @return true on success.
     */
    UA_StatusCode getEndpoints(
        const std::string&          serverUrl,
        std::vector<std::string>&   list);

    /**
     * Gets a list of all registered servers at the given server.
     *
     * You can pass an optional filter for serverUris. If the given server is not registered,
     * an empty array will be returned. If the server is registered, only that application
     * description will be returned.
     *
     * You can optionally indicate which localization you want for the server name
     * in the returned application description. The array indicates the order of preference.
     * A server may have localized names.
     *
     * The client must be connected to the same endpoint given in serverUrl or otherwise in disconnected state.
     * @param[in] serverUrl url to connect (for example "opc.tcp://localhost:4840")
     * @param[in, out] serverUris Optional filter for specific server uris
     * @param[in] localeIds Optional indication of the localization preference order.
     * @param[out] registeredServers array containing found/registered servers
     * @return true on success.
     */
    bool findServers(
        const std::string&           serverUrl,
        const StringArray&           serverUris,
        const StringArray&           localeIds,
        ApplicationDescriptionArray& registeredServers);

    /**
    /* Get a list of all known server in the network. Only supported by LDS servers.
    *
     * The client must be connected to the same endpoint given in serverUrl or otherwise in disconnected state.
    * @param[in] serverUrl url to connect (for example "opc.tcp://localhost:4840")
    * @param[in] startingRecordId optional.
    *            Only return the records with an ID higher or equal the given one.
    *            Can be used for pagination to only get a subset of the full list
    * @param[in] maxRecordsToReturn optional. Only return this number of records.
    * @param[in] serverCapabilityFilter optional. Filter the returned list to only get
    *            servers with given capabilities, e.g. "LDS"
    * @param[out] serverOnNetwork array containing known/registered servers.
     * @return true on success.
     */
    bool findServersOnNetwork(
        const std::string&      serverUrl,
        unsigned                startingRecordId,
        unsigned                maxRecordsToReturn,
        const StringArray&      serverCapabilityFilter,
        ServerOnNetworkArray&   serverOnNetwork);
    
    ///////////////////////////////////////////////////////////////////////////
    // Attributes accessors
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Primitive used to retrieve one attribute of a given node, thread-safely.
     * @warning Don't use it directly. Use one of the 19 typed version instead, like readNodeId().
     * There are up to 22 possible node attributes.
     * @param nodeId to read.
     * @param attributeId identify the attribute to retrieve.
     * @param[out] value of the attribute, must be casted to attribute type.
     *             Some are UA_Boolean, U_NodeId, etc...
     * @param type of the attribute.
     * @see UA_AttributeId for the list of possible attribute id.
     * @return true on success.
     */
    bool readAttribute(
        const UA_NodeId&    nodeId,
        UA_AttributeId      attributeId,
        void*               value,
        const UA_DataType&  type);
    
    /**
     * Primitive used to write one attribute of a given node, thread-safely.
     * @warning Don't use it directly. Use one of the 13 typed version instead, like writeValue().
     * There are up to 22 possible node attributes.
     * @param nodeId to write
     * @param attributeId identify the attribute to write. 
     * @param value void pointer to the data to write.
     * @param type pointer to the attribute built-in type. Normally stored in the UA_TYPES array.
     * @see UA_AttributeId for the list of possible attribute id.
     * @see UA_TYPES for the list of possible type.
     * @return true on success.
     */
    bool writeAttribute(
        const UA_NodeId&    nodeId,
        UA_AttributeId      attributeId,
        const void*         value,
        const UA_DataType&  type);

    /**
     * Get the client connection status, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @return connection state
     */
    UA_ClientState getState();

    /**
     * Reset the UA client, thread-safely.
     * Reset its connections, synch data, async services,
     * subscription, work queue and configuration.
     * (delete, then initialize them)
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     */
    void reset();

    // Connect and Disconnect

    /**
     * Connect to the given server, thread-safely.
     * Existing connection are closed.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @param endpointUrl server url. (for example "opc.tcp://localhost:4840")
     * @return true on success.
     */
    bool connect(const std::string& endpointUrl);

    /**
     * Connect to the selected server with the given username and password, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @param endpoint server url. (for example "opc.tcp://localhost:4840")
     * @param username
     * @param password
     * @return true on success.
     */
    bool connectUsername(
        const std::string& endpoint,
        const std::string& username,
        const std::string& password);

    /**
     * Connect to the server without waiting for the server reply, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @param endpoint server url. (for example "opc.tcp://localhost:4840")
     * @return true on success.
     */
    bool connectAsync(const std::string& endpoint);

    /**
     * Connect to the server without creating a session, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @param endpoint server url. (for example "opc.tcp://localhost:4840")
     * @return true on success.
     */
    bool connectNoSession(const std::string& endpoint);

    /**
     * Disconnect and close a connection to the selected server, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @return true on success.
     */
    bool disconnect();

    /**
     * Disconnect and close a connection to the selected server, thread-safely.
     * @warning Assumes a non-null client, otherwise throws Null client exception.
     * @param[in] requestId optional. 0 by default.
     * @return true on success.
     */
    bool disconnectAsync(UA_UInt32 requestId = 0);

    /**
     * Manually Renew Secure Channel.
     * @return true on success.
     */
    bool manuallyRenewSecureChannel() { return runIterate(0); }

    /**
     * Get the namespace-index of a namespace-URI.
     * @param namespaceUri specify the namespace URI.
     * @return the namespace index of the URI. -1 in case of an error.
     */
    int namespaceGetIndex(const std::string& namespaceUri);

    /**
     * Copy the descendants tree of a given UA_NodeId into a given PropertyTree.
     * Browse the tree from a given UA_NodeId (excluded from copying)
     * and add all its children as children of the given UANode.
     * @param[in] root parent of the nodes to copy.
     * @param[in, out] dest destination point in tree to which children nodes are added.
     * @return true on success.
     */
    bool browseTree(const UA_NodeId& root, UANode* dest);

    /**
     * Copy a NodeId and its descendants into a UANodeTree.
     * Replace the root of the given tree by a copy of the given node
     * and all its descendants.
     * Browse the tree from the given NodeId (included in copying)
     * and add all its children as children of the given UANodeTree's root.
     * Produces an addressable tree using dot separated browse path as key.
     * UANodeTree is a specialized PropertyTree using node name as key and NodeId as value.
     * @param[in] nodeId source from which browsing starts in the source tree. It isn't copied, only its children.
     * @param[out] tree the destination UANodeTree. Its root isn't modified.
     * @return true on success.
     */
    bool browseTree(const NodeId& nodeId, UANodeTree& tree);

    /**
     * Copy a NodeId and its descendants tree into a NodeIdMap.
     * NodeIdMap maps a serialized UA_NodeId as key with the UA_NodeId itself as value.
     * @param[in] nodeId the starting point added to the map with its children.
     * @param[out] map the destination NodeIdMap.
     * @return true on success.
     */
    bool browseTree(const NodeId& nodeId, NodeIdMap& map);

    /**
     * Copy only the non-duplicate children of a UA_NodeId into a NodeIdMap.
     * NodeIdMap maps a serialized UA_NodeId as key with the UA_NodeId itself as value.
     * @param[in] nodeId parent of children to copy
     * @param[out] map to fill
     * @return true on success.
     */
    bool browseChildren(const UA_NodeId& nodeId, NodeIdMap& map);

    /**
     * Get the node id from the path of browse names in the given namespace. Tests for node existence
     * @param[in] start the reference node for the path
     * @param[in] path relative to start
     * @param[out] nodeId the found node
     * @return true on success, otherwise nodeId refer to the last node matching the path.
     */
    bool nodeIdFromPath(const NodeId& start, const Path& path, NodeId& nodeId);

    /**
     * Create folder path first then add variable node to path's end leaf
     * @param[in] start the reference node for the path
     * @param[in] path relative to start
     * @param[in] nameSpaceIndex namespace of the created node.
     * @param[out] nodeId is a shallow copy - do not delete and is volatile
     * @return true on success.
     */
    bool createFolderPath(const NodeId& start, const Path& path, int idxNameSpace, NodeId& nodeId);

    /**
     * Get the child with a specific name of a given node.
     * @param start the parent node
     * @param childName the name of the child node to find.
     * @param[out] the found node.
     * @return true on success.
     */
    bool getChild(const NodeId& start, const std::string& childName, NodeId& ret);

    /**
     * Get the list of children of a node, thread-safely.
     * @param node the id of the node.
     * @return a vector of UA_NodeId containing the list of all the node's children.
     */
    UANodeIdList getChildrenList(const UA_NodeId& node);

    // Attribute access generated from the docs

    /**
     * Read the Id attribute of a given node, thread-safely.
     * Permit to test if the node exists, since its id is already known
     * @param nodeId of the node to read.
     * @param[out] outNodeId the id of the node.
     * @return true on success.
     */
    bool readNodeId(const UA_NodeId& nodeId, UA_NodeId& outNodeId) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_NODEID,
                            &outNodeId, UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * Read the Node Class attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outNodeClass the node type (object, variable, method, object type, variable type, reference, data, view)
     * @see UA_NodeClass the enum mask encoding the node type
     * @return true on success.
     */
    bool readNodeClass(const UA_NodeId& nodeId, UA_NodeClass& outNodeClass) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_NODECLASS,
                            &outNodeClass, UA_TYPES[UA_TYPES_NODECLASS]);
    }

    /**
     * Read the Browse Name attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outBrowseName the browse name of the node.
     * @return true on success.
     */
    bool readBrowseName(const UA_NodeId& nodeId, QualifiedName& outBrowseName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME,
                            &outBrowseName, UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
    }

    /**
     * Retrieve the browse name and the namespace of a given node.
     * @param nodeId
     * @param[out] outName the node's browse name
     * @param[out] outNamesapce the node's namespace
     * @return true on success. On failure the output params are unchanged.
     */
    bool readBrowseName(const NodeId& nodeId, std::string& outName, int& outNamespace);

    /**
     * Read the Display Name attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outDisplayName the display name of the node, translated in the local language.
     * @return true on success.
     */
    bool readDisplayName(const UA_NodeId& nodeId, LocalizedText& outDisplayName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME,
                            &outDisplayName, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * Read the Description attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outDescription the node description, translated in the local language.
     * @return true on success.
     */
    bool readDescription(const UA_NodeId& nodeId, LocalizedText& outDescription) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION,
                            &outDescription, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * Read the Write Mask attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outWriteMask specify which attribute of the node can be modified
     * @return true on success.
     */
    bool readWriteMask(const UA_NodeId& nodeId, UA_UInt32& outWriteMask) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK,
                            &outWriteMask, UA_TYPES[UA_TYPES_UINT32]);

    }

    /**
     * Read the User Write Mask attribute of a given node, thread-safely.
     * @param nodeId of the node to read.
     * @param[out] outWriteMask specify which attribute of the node can be modified
     * @return true on success.
     */
    bool readUserWriteMask(const UA_NodeId& nodeId, UA_UInt32& outUserWriteMask) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_USERWRITEMASK,
                            &outUserWriteMask, UA_TYPES[UA_TYPES_UINT32]);

    }

    /**
     * Read the Is Abstract attribute of a given node, thread-safely.
     * Only for Data, Object Type, Variable Type or Reference nodes.
     * @param nodeId of the node to read.
     * @param[out] outIsAbstract true if the node is abstract.
     *             ie: UA_Numeric is abstract, UA_Int32 is not and is a concrete implementation of UA_Numeric.
     * @return true on success.
     */
    bool readIsAbstract(const UA_NodeId& nodeId, UA_Boolean& outIsAbstract) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT,
                            &outIsAbstract, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Read the Symmetric attribute of a given node, thread-safely.
     * Only for Reference nodes.
     * @param nodeId of the node to read.
     * @param[out] outSymmetric true if the reference applies both to the child and parent.
     * @return true on success.
     */
    bool readSymmetric(const UA_NodeId& nodeId, UA_Boolean& outSymmetric) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_SYMMETRIC,
                            &outSymmetric, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Read the Inverse Name attribute of a given node, thread-safely.
     * Only for Reference nodes.
     * @param nodeId of the node to read.
     * @param[out] outInverseName
     * @return true on success.
     */
    bool readInverseName(const UA_NodeId& nodeId, LocalizedText& outInverseName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME,
                            &outInverseName, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * Read the Contains No Loop attribute of a given node, thread-safely.
     * Only for View nodes.
     * @param nodeId of the node to read.
     * @param[out] outContainsNoLoops
     * @return true on success.
     */
    bool readContainsNoLoops(const UA_NodeId& nodeId, UA_Boolean& outContainsNoLoops) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_CONTAINSNOLOOPS,
                            &outContainsNoLoops, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Read the Event Notifier attribute of a given node, thread-safely.
     * Only for Object and View nodes.
     * @param nodeId of the node to read.
     * @param[out] outEventNotifier
     * @return true on success.
     */
    bool readEventNotifier(const UA_NodeId& nodeId, UA_Byte& outEventNotifier) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EVENTNOTIFIER,
                            &outEventNotifier, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Read the Value attribute of a given node, thread-safely.
     * Only for Variable and Variable Type nodes.
     * @param nodeId of the node to read.
     * @param[out] outValue the value of the variable node.
     * @return true on success.
     */
    bool readValue(const UA_NodeId& nodeId, Variant& outValue) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                            &outValue, UA_TYPES[UA_TYPES_VARIANT]);
    }

    /**
     * Read the Data Type attribute of a given node, thread-safely.
     * Only for Variable and Variable Type nodes.
     * @param nodeId of the node to read.
     * @param[out] outDataType the type of the data of the variable node.
     * @return true on success.
     */
    bool readDataType(const UA_NodeId& nodeId, UA_NodeId& outDataType) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE,
                            &outDataType, UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * Read the Value Rank attribute of a given node, thread-safely.
     * Only for Variable and Variable Type nodes.
     * @param nodeId of the node to read.
     * @param[out] outValueRank indicates whether the variable is an array
     *             and how many dimensions the array has.
     * @return true on success.
     * @see https://open62541.org/doc/current/nodestore.html?highlight=writemask#value-rank
     */
    bool readValueRank(const UA_NodeId& nodeId, UA_Int32& outValueRank) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK,
                            &outValueRank, UA_TYPES[UA_TYPES_INT32]);
    }

    /**
     * Read the Array Dimensions attribute of a given node, thread-safely.
     * Only for Variable and Variable Type nodes.
     * @param nodeId of the node to read.
     * @param[out] outArrayDimensions a variant with an int32 array containing the size of each dimension
     *             ie. if ValueRank is 3, ArrayDimensions can be something link {2, 2, 3}
     * @return true on success.
     * @see https://open62541.org/doc/current/nodestore.html?highlight=writemask#array-dimensions
     */
    bool readArrayDimensions(const UA_NodeId& nodeId, std::vector<UA_UInt32>& ret);

    /**
     * Read the Access Level attribute of a given node, thread-safely.
     * Only for Variable nodes.
     * @param nodeId of the node to read.
     * @param[out] outAccessLevel a mask specifying if the value can be read/written, its history read/written, etc...
     * @return true on success.
     * @see UA_ACCESSLEVELMASK_READ, UA_ACCESSLEVELMASK_WRITE, UA_ACCESSLEVELMASK_HISTORYREAD, UA_ACCESSLEVELMASK_HISTORYWRITE
     *      UA_ACCESSLEVELMASK_SEMANTICCHANGE, UA_ACCESSLEVELMASK_STATUSWRITE, UA_ACCESSLEVELMASK_TIMESTAMPWRITE
     */
    bool readAccessLevel(const UA_NodeId& nodeId, UA_Byte& outAccessLevel) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL,
                             &outAccessLevel, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Read the User Access Level attribute of a given node, thread-safely.
     * Only for Variable nodes.
     * @param nodeId of the node to read.
     * @param[out] outAccessLevel a mask specifying if the value can be read/written, its history read/written, etc...
     * @return true on success.
     * @see UA_ACCESSLEVELMASK_READ, UA_ACCESSLEVELMASK_WRITE, UA_ACCESSLEVELMASK_HISTORYREAD, UA_ACCESSLEVELMASK_HISTORYWRITE
     *      UA_ACCESSLEVELMASK_SEMANTICCHANGE, UA_ACCESSLEVELMASK_STATUSWRITE, UA_ACCESSLEVELMASK_TIMESTAMPWRITE
     */
    bool readUserAccessLevel(const UA_NodeId& nodeId, UA_Byte& outUserAccessLevel) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_USERACCESSLEVEL,
                            &outUserAccessLevel, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Read the Minimum Sampling Interval attribute of a given node, thread-safely.
     * Only for Variable nodes.
     * @param nodeId of the node to read.
     * @param[out] outMinInterval the value Minimum Sampling Interval.
     * @return true on success.
     */
    bool readMinimumSamplingInterval(const UA_NodeId& nodeId, UA_Double& outMinSamplingInterval) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                            &outMinSamplingInterval, UA_TYPES[UA_TYPES_DOUBLE]);
    }

    /**
     * Read the Historizing attribute of a given node, thread-safely.
     * Only for Variable nodes.
     * @param nodeId of the node to read.
     * @param[out] outHistorizing true if the variable node is keeping its value history.
     * @return true on success.
     */
    bool readHistorizing(const UA_NodeId& nodeId, UA_Boolean& outHistorizing) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_HISTORIZING,
                            &outHistorizing, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Read the Executable attribute of a given node, thread-safely.
     * Only for method nodes.
     * @param nodeId of the node to read.
     * @param[out] outExecutable true if the method is active and can be executed.
     * @return true on success.
     */
    bool readExecutable(const UA_NodeId& nodeId, UA_Boolean& outExecutable) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE,
                            &outExecutable, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Read the User Executable attribute of a given node, thread-safely.
     * Only for method nodes.
     * @param nodeId of the node to read.
     * @param[out] outExecutable true if the method is active and can be executed.
     * @return true on success.
     */
    bool readUserExecutable(const UA_NodeId& nodeId, UA_Boolean& outUserExecutable) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_USEREXECUTABLE,
                            &outUserExecutable, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    ///////////////////////////////////////////////////////////////////////////

    /**
     * Set the NodeId attribute of the given node, thread-safely.
     * @param nodeId
     * @param newNodeId
     * @return true on success.
     */
    bool setNodeId(NodeId& nodeId, const NodeId& newNodeId) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_NODEID,
                             &newNodeId, UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * Set the NodeClass attribute of the given node, thread-safely.
     * @param nodeId
     * @param newNodeClass
     * @return true on success.
     */
    bool setNodeClass(NodeId& nodeId, const UA_NodeClass& newNodeClass) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_NODECLASS,
                              &newNodeClass, UA_TYPES[UA_TYPES_NODECLASS]);
    }

    /**
     * Set the BrowseName attribute of the given node, thread-safely.
     * @param nodeId
     * @param newBrowseName
     * @return true on success.
     */
    bool setBrowseName(NodeId& nodeId, const QualifiedName& newBrowseName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME,
                              &newBrowseName, UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
    }

    /**
     * Set the BrowseName of a node with the given namespace and name, thread-safely.
     * @param nodeId to modify
     * @param nameSpaceIndex part of the new browse name
     * @param name
     */
    void setBrowseName(NodeId& nodeId, int nameSpaceIndex, const std::string& name);

    /**
     * Set the DisplayName attribute of the given node, thread-safely.
     * @param nodeId
     * @param newDisplayName
     * @return true on success.
     */
    bool setDisplayName(NodeId& nodeId, const LocalizedText& newDisplayName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME,
                              &newDisplayName, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * Set the Description attribute of the given node, thread-safely.
     * @param nodeId
     * @param newDescription
     * @return true on success.
     */
    bool setDescription(NodeId& nodeId, const LocalizedText& newDescription) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION,
                              &newDescription, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
    * Set the WriteMask attribute of the given node, thread-safely.
    * @param nodeId
    * @param newWriteMask
    * @return true on success.
    */
    bool setWriteMask(NodeId& nodeId, UA_UInt32 newWriteMask) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK,
                              &newWriteMask, UA_TYPES[UA_TYPES_UINT32]);
    }

    /**
    * Set the User WriteMask attribute of the given node, thread-safely.
    * @param nodeId
    * @param newUserWriteMask
    * @return true on success.
    */
    bool setUserWriteMask(NodeId& nodeId, UA_UInt32 newUserWriteMask) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_USERWRITEMASK,
                              &newUserWriteMask, UA_TYPES[UA_TYPES_UINT32]);
    }

    /**
     * Set the IsAbstract attribute of the given node, thread-safely.
     * @param nodeId
     * @param newIsAbstract
     * @return true on success.
     */
    bool setIsAbstract(NodeId& nodeId, UA_Boolean newIsAbstract) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT,
                              &newIsAbstract, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Set the Symmetric attribute of the given node, thread-safely.
     * @param nodeId
     * @param newSymmetric
     * @return true on success.
     */
    bool setSymmetric(NodeId& nodeId, UA_Boolean newSymmetric) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_SYMMETRIC,
                              &newSymmetric, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Set the InverseName attribute of the given node, thread-safely.
     * @param nodeId
     * @param newInverseName
     * @return true on success.
     */
    bool setInverseName(NodeId& nodeId, const LocalizedText& newInverseName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME,
                              &newInverseName, UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    /**
     * Set the ContainsNoLoops attribute of the given node, thread-safely.
     * @param nodeId
     * @param newContainsNoLoops
     * @return true on success.
     */
    bool setContainsNoLoops(NodeId& nodeId, UA_Boolean newContainsNoLoops) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_CONTAINSNOLOOPS,
                              &newContainsNoLoops, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Set the EventNotifier attribute of the given node, thread-safely.
     * @param nodeId
     * @param newEventNotifier
     * @return true on success.
     */
    bool setEventNotifier(NodeId& nodeId, UA_Byte newEventNotifier) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_EVENTNOTIFIER,
                             &newEventNotifier, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Set the Value attribute of the given node, thread-safely.
     * @param nodeId
     * @param newValue
     * @return true on success.
     */
    bool setValue(NodeId& nodeId, const Variant& newValue) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                              &newValue, UA_TYPES[UA_TYPES_VARIANT]);
    }

    /**
     * Set the DataType attribute of the given node, thread-safely.
     * @param nodeId
     * @param newDataType
     * @return true on success.
     */
    bool setDataType(NodeId& nodeId, const UA_NodeId& newDataType) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE,
                              &newDataType, UA_TYPES[UA_TYPES_NODEID]);
    }

    /**
     * Set the ValueRank attribute of the given node, thread-safely.
     * @param nodeId
     * @param newValueRank
     * @return true on success.
     */
    bool setValueRank(NodeId& nodeId, UA_Int32 newValueRank) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK,
                              &newValueRank, UA_TYPES[UA_TYPES_INT32]);
    }

    /**
     * Set the ArrayDimensions attribute of the given node, thread-safely.
     * @param nodeId
     * @param newArrayDimensions
     * @return true on success.
     */
    bool setArrayDimensions(
        NodeId&                 nodeId,
        std::vector<UA_UInt32>& newArrayDimensions);

    /**
     * Set the AccessLevel attribute of the given node, thread-safely.
     * @param nodeId
     * @param newAccessLevel
     * @return true on success.
     */
    bool setAccessLevel(NodeId& nodeId, UA_Byte newAccessLevel) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL,
                              &newAccessLevel, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Set the UserAccessLevel attribute of the given node, thread-safely.
     * @param nodeId
     * @param newUserAccessLevel
     * @return true on success.
     */
    bool setUserAccessLevel(NodeId& nodeId, UA_Byte newUserAccessLevel) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_USERACCESSLEVEL,
                              &newUserAccessLevel, UA_TYPES[UA_TYPES_BYTE]);
    }

    /**
     * Set the MinimumSamplingInterval attribute of the given node, thread-safely.
     * @param nodeId
     * @param newMinInterval
     * @return true on success.
     */
    bool setMinimumSamplingInterval(NodeId& nodeId, UA_Double newMinInterval) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                              &newMinInterval, UA_TYPES[UA_TYPES_DOUBLE]);
    }

    /**
     * Set the Historizing attribute of the given node, thread-safely.
     * @param nodeId
     * @param newHistorizing
     * @return true on success.
     */
    bool setHistorizing(NodeId& nodeId, UA_Boolean newHistorizing) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_HISTORIZING,
                              &newHistorizing, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Set the Executable attribute of the given node, thread-safely.
     * @param nodeId
     * @param newExecutable
     * @return true on success.
     */
    bool setExecutable(NodeId& nodeId, UA_Boolean newExecutable) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE,
                              &newExecutable, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /**
     * Set the UserExecutable attribute of the given node, thread-safely.
     * @param nodeId
     * @param newUserExecutable
     * @return true on success.
     */
    bool setUserExecutable(NodeId& nodeId, UA_Boolean newUserExecutable) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_USEREXECUTABLE,
                              &newUserExecutable, UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    // End of attributes

    /**
    * Deletes a node and optionally all references leading to the node, thread-safely.
    * @param nodeId to delete
    * @param deleteReferences specify if the references to this node must also be deleted.
    * @return true on success.
     */
    bool deleteNode(const NodeId& nodeId, bool deleteReferences);

    /**
    * Delete a node and all its descendants
    * @param nodeId node to be deleted with its children
    * @return true on success.
     */
    bool deleteTree(const NodeId& nodeId); // recursive delete

    /**
     * Client::deleteChildren
     * @param n
     */
    void deleteChildren(const UA_NodeId& n);

    /**
     * Call a given server method, thread-safely.
     * @param[in] objectId
     * @param[in] methodId
     * @param[in] in Array of variants with the method input arguments.
     * @param[out] out method outputs. Support multiple return values.
     * @return true on success.
     */
    bool callMethod(
        const NodeId&       objectId,
        const NodeId&       methodId,
        const VariantList&  in,
        VariantArray&       out);

    /**
     * Hook to customize the Periodic processing.
     * Do nothing by default.
     * If a client need to do something periodically,
     * override this method and add the client to a client cache thread.
     * @return true on success
     * @see ClientCacheThread and ClientCache classes
     */
    virtual bool process() { return true; }

    // Add nodes - templated from docs

    /**
     * Add a children Folder node in the server, thread-safely.
     * @param parent parent node
     * @param childName browse name of the folder node
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param outNewNodeId receives new node if not null
     * @param nameSpaceIndex of the new node, if non-zero otherwise namespace of parent
     * @return true on success.
     */
    bool addFolder(
        const NodeId&       parent,
        const std::string&  childName,
        const NodeId&       nodeId,
        NodeId&             outNewNodeId    = NodeId::Null,
        int                 nameSpaceIndex  = 0);
    
    /**
     * Add a new variable node in the server, thread-safely.
     * @param parent specify the parent node containing the added node
     * @param name browse name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param outNewNodeId receives new node if not null
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success.
     */
    bool addVariable(
        const NodeId&       parent,
        const std::string&  name,
        const Variant&      value,
        const NodeId&       nodeId,
        NodeId&             outNewNodeId    = NodeId::Null,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a new property node in the server, thread-safely.
     * @param parent specify the parent node containing the added node.
     * @param name browse name of the new node.
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign.
     * @param[out] outNewNodeId receives new node if not null.
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent.
     * @return true on success.
    */
    bool addProperty(
        const NodeId&       parent,
        const std::string&  name,
        const Variant&      value,
        const NodeId&       nodeId          = NodeId::Null,
        NodeId&             outNewNodeId    = NodeId::Null,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a new variable type node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param typeDefinition nodeId containing the definition of the type of the added node.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addVariableTypeNode(
        const NodeId&                 requestedNewNodeId,
        const NodeId&                 parentNodeId,
        const NodeId&                 referenceTypeId,
        const QualifiedName&          browseName,
        const VariableTypeAttributes& attr,
        NodeId&                       outNewNodeId = NodeId::Null);

    /**
     * Add a new object node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param typeDefinition nodeId containing the definition of the type of the added node.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addObjectNode(
        const NodeId&             requestedNewNodeId,
        const NodeId&             parentNodeId,
        const NodeId&             referenceTypeId,
        const QualifiedName&      browseName,
        const NodeId&             typeDefinition,
        const ObjectAttributes&   attr,
        NodeId&                   outNewNodeId = NodeId::Null);

    /**
     * Add a new object type node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addObjectTypeNode(
        const NodeId&                 requestedNewNodeId,
        const NodeId&                 parentNodeId,
        const NodeId&                 referenceTypeId,
        const QualifiedName&          browseName,
        const ObjectTypeAttributes&   attr,
        NodeId&                       outNewNodeId = NodeId::Null);

    /**
     * Add a new view node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addViewNode(
        const NodeId&         requestedNewNodeId,
        const NodeId&         parentNodeId,
        const NodeId&         referenceTypeId,
        const QualifiedName&  browseName,
        const ViewAttributes& attr,
        NodeId&               outNewNodeId = NodeId::Null);

    /**
     * Add a new reference type node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addReferenceTypeNode(
        const NodeId&                  requestedNewNodeId,
        const NodeId&                  parentNodeId,
        const NodeId&                  referenceTypeId,
        const QualifiedName&           browseName,
        const ReferenceTypeAttributes& attr,
        NodeId&                        outNewNodeId = NodeId::Null);

    /**
     * Add a new data type node in the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addDataTypeNode(
        const NodeId&             requestedNewNodeId,
        const NodeId&             parentNodeId,
        const NodeId&             referenceTypeId,
        const QualifiedName&      browseName,
        const DataTypeAttributes& attr,
        NodeId&                   outNewNodeId = NodeId::Null);

    /**
     * Add a new method node to the server, thread-safely.
     * @param requestedNewNodeId assigned node id or NodeId::Null for auto assign
     * @param parentNodeId parent node id of the added node.
     * @param referenceTypeId specify the relation between the added node and its children.
     *        ie: NodeId::HasSubType or NodeId::HasComponent.
     * @param browseName browse name of the added node, includes the namespace.
     * @param attr the attributes of the added node.
     * @param outNewNodeId receives new node if not null.
     * @return true on success.
     */
    bool addMethodNode(
        const NodeId&             requestedNewNodeId,
        const NodeId&             parentNodeId,
        const NodeId&             referenceTypeId,
        const QualifiedName&      browseName,
        const MethodAttributes&   attr,
        NodeId&                   outNewNodeId = NodeId::Null);

    /**
     * Hook customizing and simplify the historicalIteratorCallback call-back
     * used in historyReadRaw
     * @return 
     */
    virtual bool historicalIterator(
        const NodeId&               node,
        UA_Boolean                  moreDataAvailable,
        const UA_ExtensionObject&   data) { return false; }

    /**
     * Call-back used to iterate over historical data of a node.
     * Used in historyReadRaw.
     * @param client requesting the reading (not used, assume this is the client)
     * @param nodeId being read
     * @param moreDataAvailable flag true if more data can be read
     * @param[out] data the read data.
     * @param callbackContext point on a Client which historicalIterator() hook
     *        will be used for the iteration.
     *        Permits to borrow the iteration process defined in another client.
     * @return 
     */
    static UA_Boolean historicalIteratorCallback(
        UA_Client*                  client,
        const UA_NodeId*            nodeId,
        UA_Boolean                  moreDataAvailable,
        const UA_ExtensionObject*   data,
        void*                       callbackContext);

    /**
     * Read the historical data value of a given node
     * in a given timestamp range.
     * @param node id of the node for which historical data is requested.
     * @param start is the start of the timestamp range.
     * @param end is the end of the timestamp range.
     * @param numValuesPerNode is the maximum number of items per response the client wants to receive.
     * @param indexRange specify the starting index (should probably not be used)
     * @param returnBounds determines if the client wants to receive bounding values.
     * @param timestampsToReturn specify which time stamps the client is interested in;
     *        device, server or both. @see UA_TimestampsToReturn
     * @return 
     */
    bool historyReadRaw(
        const NodeId&           node,
        UA_DateTime             start,
        UA_DateTime             end,
        unsigned                numValuesPerNode,
        const UA_String&        indexRange          = UA_STRING_NULL,
        bool                    returnBounds        = false,
        UA_TimestampsToReturn   timestampsToReturn  = UA_TIMESTAMPSTORETURN_BOTH);

    /**
     * Add a new data value in a node's history.
     * @param node to modify.
     * @param value to insert.
     * @return true on success.
     */
    bool historyUpdateInsert(const NodeId& node, const UA_DataValue& value);

    /**
     * Replace a data value in a node's history. Time stamp modified?
     * @param node to modify.
     * @param value to insert.
     * @return true on success.
     */
    bool historyUpdateReplace(const NodeId& node, const UA_DataValue& value);

    /**
     * Update a data value in a node's history. Time stamps not modified?
     * @param node to modify.
     * @param value to insert.
     * @return true on success.
     */
    bool historyUpdateUpdate(const NodeId& node, const UA_DataValue& value);

    /**
     * Delete the data value in a node's history in a timestamp range.
     * @param node to modify.
     * @param startTimestamp specify the begining of the range.
     * @return endTimestamp specify the end of the range.
     */
    bool historyUpdateDeleteRaw(
        const NodeId&   node,
        UA_DateTime     startTimestamp,
        UA_DateTime     endTimestamp);
};

} // namespace Open62541

#endif // OPEN62541CLIENT_H
