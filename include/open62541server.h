/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef OPEN62541SERVER_H
#define OPEN62541SERVER_H

#include "open62541objects.h"
#include "nodecontext.h"
#include "servermethod.h"
#include "serverrepeatedcallback.h"

namespace Open62541 {

class HistoryDataGathering;
class HistoryDataBackend;

/**
 * The Server class abstracts the server side.
 * This class wraps the corresponding C functions. Refer to the C documentation for a full explanation.
 * The main thing to watch for is Node ID objects are passed by reference. There are stock Node Id objects including NodeId::Null
 * Pass NodeId::Null where a NULL UA_NodeId pointer is expected.
 * If a NodeId is being passed to receive a value use the notNull() method to mark it as a receiver of a new node id.
 * Most functions return true if the lastError is UA_STATUSCODE_GOOD.
 */
class UA_EXPORT Server {
    using CallBackList = std::map<std::string, SeverRepeatedCallbackRef>;   /**< Map call-backs names to a repeated call-back shared pointers. */
    using ServerMap    = std::map<UA_Server*, Server*>;                     /**< Map UA_Server pointer key to servers pointer value */
    using DiscoveryMap = std::map<UA_UInt64, std::string>;                  /**< Map the repeated registering call-back id with the discovery server URL */
    using LoginList    = std::vector<UA_UsernamePasswordLogin>;

    UA_Server*          _server   = nullptr;    /**< assume one server per application */
    UA_ServerConfig*    _config   = nullptr;    /**< The server configuration */
    UA_Boolean          _running  = false;      /**< Flag both used to keep the server running and storing the server status. Set it to false to stop the server. @see stop(). */
    CallBackList        _callbacks;             /**< Map call-backs names to a repeated call-back shared pointers. */
    ReadWriteMutex      _mutex;                 /**< mutex for thread-safe read-write of the server nodes. Should probably mutable */

    static ServerMap    _serverMap;             /**< map UA_SERVERs to Server objects. Enables a server to find another one. */
    DiscoveryMap        _discoveryList;         /**< set of discovery servers this server has registered with.
                                                     Map the repeated registering call-back id with the discovery server URL. */
    LoginList           _logins;                /**< set of permitted logins (user, password pairs)*/


    // Life cycle call-backs

    /**
     * Call-back used to construct a new node in a given server.
     * Can be NULL. May replace the nodeContext.
     * nodeContext.construct() is used to to build the new node, taking nodeId as input.
     * @param server specify the server of the new node.
     * @param sessionId specify the session id (currently unused)
     * @param sessionContextspecify the session context (currently unused)
     * @param nodeId the node copied by the constructed node
     * @param nodeContext specify how to construct the node via its construct() method.
     * @return UA_STATUSCODE_GOOD on success or nothing done, UA_STATUSCODE_BADINTERNALERROR construction failure.
     * @see NodeContext
     */
    static UA_StatusCode constructor(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void** nodeContext);

    /**
     * Call-back used to destroy a node in a given server.
     * Can be NULL. The context cannot be replaced since
     * the node is destroyed immediately afterwards anyway.
     * nodeContext.destruct() is used to to destroy the node, taking nodeId as input.
     * @param server specify the server of the node to destroy
     * @param sessionId specify the session id (currently unused)
     * @param sessionContextspecify the session context (currently unused)
     * @param nodeId used to identify the node to destroy
     * @param nodeContext specify how to destroy the node via its destruct() method.
     */
    static void destructor(
        UA_Server* server,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext);

    // Access Control call-backs - these invoke virtual functions to control access
    /**
     * Control access to the Add Node function on a given server.
     * Behavior modified by overriding allowAddNode() hook.
     * Allowed by default if not specialized.
     * @return true if access is granted.
     */
    static UA_Boolean allowAddNodeHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_AddNodesItem* item);

    /**
     * Control access to the Add Node Reference function on a given server.
     * Behavior modified by overriding allowAddReference() hook.
     * Allowed by default if not specialized.
     * @return true if access is granted.
     */
    static UA_Boolean allowAddReferenceHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_AddReferencesItem* item);

    /**
     * Control access to the Delete Node function on a given server.
     * Behavior modified by overriding allowDeleteNode() hook.
     * Not allowed by default if not specialized.
     * @return true if access is granted.
     */
    static UA_Boolean allowDeleteNodeHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_DeleteNodesItem* item);

    /**
     * Control access to the Delete Node Reference function on a given server.
     * Behavior modified by overriding allowDeleteReference() hook.
     * Allowed by default if not specialized.
     * @return true if access is granted.
     */
    static UA_Boolean allowDeleteReferenceHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_DeleteReferencesItem* item);

    /**
     * Control access to the Activate Session function on a given server.
     * Behavior modified by overriding activateSession() hook.
     * Not allowed by default if not specialized.
     * @return  UA_STATUSCODE_GOOD on success.
                UA_STATUSCODE_BADSESSIONIDINVALID or any relevant error on refusal.
                -1 if the server wasn't found.
     */
    static UA_StatusCode activateSessionHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_EndpointDescription* endpointDescription,
        const UA_ByteString* secureChannelRemoteCertificate,
        const UA_NodeId* sessionId,
        const UA_ExtensionObject* userIdentityToken,
        void** sessionContext);

    /**
     * De-authenticate a session and cleanup.
     * Behavior modified by overriding closeSession() hook.
     */
    static void closeSessionHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext);

    /**
     * Access control for all nodes.
     * Behavior modified by overriding getUserRightsMask() hook.
     * Allowed by default.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static UA_UInt32 getUserRightsMaskHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext);

    /**
     * Additional access control for variable nodes.
     * Behavior modified by overriding getUserAccessLevel() hook.
     * Allowed by default.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static UA_Byte getUserAccessLevelHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,    void* nodeContext);

    /**
     * Additional access control for method nodes.
     * Behavior modified by overriding getUserExecutable() hook.
     * Not allowed by default.
     * @return UA_TRUE on success.
     */
    static UA_Boolean getUserExecutableHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* methodId,  void* methodContext);

    /**
     * Additional access control for calling a method node
     * in the context of a specific object.
     * Behavior modified by overriding getUserExecutableOnObject() hook.
     * Not allowed by default.
     * @return UA_TRUE on success.
     */
    static UA_Boolean getUserExecutableOnObjectHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* methodId,  void* methodContext,
        const UA_NodeId* objectId,  void* objectContext);

    /**
     * Allow insert,replace,update of historical data.
     * Behavior modified by overriding allowHistoryUpdateUpdateData() hook.
     * Not allowed by default.
     * @return UA_TRUE on success.
     */
    static UA_Boolean allowHistoryUpdateUpdateDataHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,
        UA_PerformUpdateType performInsertReplace,
        const UA_DataValue* value);

    /**
     * Allow delete of historical data.
     * Behavior modified by overriding allowHistoryUpdateDeleteRawModified() hook.
     * Not allowed by default.
     * @return UA_TRUE on success.
     */
    static UA_Boolean allowHistoryUpdateDeleteRawModifiedHandler(
        UA_Server* server, UA_AccessControl* ac,
        const UA_NodeId* sessionId, void* sessionContext,
        const UA_NodeId* nodeId,
        UA_DateTime startTimestamp,
        UA_DateTime endTimestamp,
        bool isDeleteModified);

protected:
    UA_StatusCode _lastError = UA_STATUSCODE_GOOD;

public:
    /**
     * Default Server constructor using default configuration.
     * Default config listen on default port 4840 with no server certificate.
     * @see UA_ServerConfig_setDefault().
     */
    Server();

    /**
     * Server constructor using minimal configuration for one endpoint.
     * Minimal config sets the TCP network layer to listen on the given port and 
     * adds a single endpoint with the security policy ``SecurityPolicy#None``.
     * An optional server certificate may be supplied.
     * @see UA_ServerConfig_setMinimal().
     * @see UA_SECURITY_POLICY_NONE_URI
     */
    Server(int port, const UA_ByteString& certificate = UA_BYTESTRING_NULL);

    /**
     * Virtual destructor terminating the server thread-safely.
     * @warning might throw.
     */
    virtual ~Server() {
        if (_server) {
            WriteLock l(_mutex);
            terminate();
        }
    }

    /**
     * Enable Multi-cast DNS with a given hostname
     * Available only if UA_ENABLE_DISCOVERY_MULTICAST is defined.
     */
    void setMdnsServerName(const std::string& name) {
        if (_config) {
            _config->discovery.mdnsEnable = true;

    #ifdef UA_ENABLE_DISCOVERY_MULTICAST
            _config->discovery.mdns.mdnsServerName = UA_String_fromChars(name.c_str());
    #else
            (void)name;
    #endif
        }
    }

    /**
     * Return a reference to the login vector member.
     * @todo creat clear(), add(), delete() update
     * @return a std::vector of user name / passwords by reference
     * @see LoginList
     */
    LoginList& logins() { return _logins; }

    /**
     * Set the list of endpoints for the server.
     * @param endpoints the new list of endpoints for the server, stored in its config.
     */
    void applyEndpoints(EndpointDescriptionArray& endpoints) {
        _config->endpoints = endpoints.data();
        _config->endpointsSize = endpoints.length();
        // Transfer ownership
        endpoints.release();
    }

    /**
     * Reset the server configuration.
     * @param endpoints the new list of endpoints for the server, stored in its config.
     */
    void configClean() {
        if (_config) UA_ServerConfig_clean(_config);
    }

    /**
     * Set up for simple login
     * assumes the permitted logins have been set up beforehand.
     * This gives username / password access and disables anonymous access
     * @return true on success 
     */
    bool enableSimpleLogin();

    /**
     * Set a custom hostname in server configuration
     */
    void setCustomHostname(const std::string& customHostname) {
        UA_String s = toUA_String(customHostname); // shallow copy
        UA_ServerConfig_setCustomHostname(_config, s);
    }

    /**
     * Set the server URI (uniform resource identifier).
     * @see UA_ApplicationDescription.
     */
    void setServerUri(const std::string& s) {
        UA_String_deleteMembers(&_config->applicationDescription.applicationUri);
        _config->applicationDescription.applicationUri = UA_String_fromChars(s.c_str());
    }

    /**
     * Find an existing Server by its UA_Server pointer.
     * Used by call-backs to verify the server exists and is still running.
     * @param s a pointer on the Server underlying UA_Server.
     * @return a pointer on the matching Server
     */
    static Server* findServer(UA_Server* s) {
        return _serverMap[s];
    }

    // Discovery

    /**
     * Register this server at the Discovery Server.
     * This should be called periodically to keep the server registered.
     * @param client used to call the RegisterServer.
     *        It must already be connected to the correct endpoint.
     * @param semaphoreFilePath optional parameter pointing to semaphore file.
     *        If the given file is deleted, the server will automatically be unregistered.
     *        This could be for example a pid file which is deleted if the server crashes.
     * @return true on success
     * @see UA_Server_register_discovery
     */
    bool registerDiscovery(Client& client, const std::string& semaphoreFilePath = "");

    /**
     * Unregister this server from the Discovery Server
     * This should only be called when the server is shutting down.
     * @param client used to call the RegisterServer.
     *        It must already be connected to the correct endpoint.
     * @return true on success
     * @see UA_Server_unregister_discovery
     */
    bool unregisterDiscovery(Client& client);

    /**
     * Adds a callback to periodically register the server with the LDS (local discovery server)
     *
     * When you manually unregister the server, you also need to cancel the
     * periodic callback, otherwise it will be automatically registered again.
     *
     * If you call this method multiple times for the same discoveryServerUrl, the older
     * periodic call-back will be removed.
     *
     * @param discoveryServerUrl where this server should register itself.
     * @param client used to call the RegisterServer. It must not yet be connected
     *        and will be connected for every register call to the given discoveryServerUrl.
     * @param periodicCallbackId return the created call-back.
     *        Map it to the discovery server URL in _discoveryList.
     * @param intervalMs specify the duration in ms between each register call.
     *        Set to 10 minutes by default (10*60*1000).
     * @param delayFirstRegisterMs indicates the delay for the first register call in ms.
     *        Set to 1s by default (1000ms).
     *        If it is 0, the first register call will be after intervalMs milliseconds,
     *        otherwise the server's first register will be after delayFirstRegisterMs.
     * @return true on success
     * @see UA_Server_addPeriodicServerRegisterCallback
     */
    bool addPeriodicServerRegister(
        const std::string&  discoveryServerUrl,
        Client&             client,
        UA_UInt64&          periodicCallbackId,
        UA_UInt32           intervalMs           = 600 * 1000, // default to 10 minutes
        UA_UInt32           delayFirstRegisterMs = 1000);
    
    /**
     * Hook used to customize registerServerCallback() by derived classes.
     * By default do nothing.
     * @see registerServerCallback
     */
    virtual void registerServer(const UA_RegisteredServer* registeredServer) {
        OPEN62541_TRC
    }

    /**
     * Call-back called every time the server gets a register call.
     * This means that for every periodic server register the callback will  be called.
     * Behavior can be customized by overriding registerServer()
     * @param registeredServer the server that (un)register itself to this one.
     * @param data
     * @see registerServer
     */
    static void registerServerCallback(const UA_RegisteredServer* registeredServer, void* data);

    /**
     * Activate the Server Registered trigger.
     * Called every time another server register or unregister with this one.
     * @see registerServer for behavior customization for this event.
     */
    void setRegisterServerCallback() {
        UA_Server_setRegisterServerCallback(server(), registerServerCallback, (void*)(this));
    }

    /**
     * Hook used to customize serverOnNetworkCallback() by derived classes.
     * By default do nothing.
     * @param serverOnNetwork
     * @param isServerAnnounce
     * @param isTxtReceived
     * @see registerServerCallback
     */
    virtual void serverOnNetwork(
        const UA_ServerOnNetwork* serverOnNetwork,
        UA_Boolean isServerAnnounce,
        UA_Boolean isTxtReceived) {
        OPEN62541_TRC
    }

    /**
     * Callback called if another server is found through mDNS or deleted.
     * It will be called for any mDNS message from the remote server, thus
     * it may be called multiple times for the same instance. Also the SRV and TXT
     * records may arrive later, therefore for the first call the server
     * capabilities may not be set yet. If called multiple times, previous data will
     * be overwritten.
     * @param serverNetwork
     * @param isServerAnnounce
     * @param isTxtReceived
     * @param data
     */
    static void serverOnNetworkCallback(
        const UA_ServerOnNetwork* serverNetwork,
        UA_Boolean isServerAnnounce,
        UA_Boolean isTxtReceived,
        void* data);

    #ifdef UA_ENABLE_DISCOVERY_MULTICAST

    /**
     * Activate the mDNS Found Server on Network trigger.
     * Called every time another server is found through mDNS or deleted.
     * @see serverOnNetwork for behavior customization for this event.
     */
    void setServerOnNetworkCallback() {
        UA_Server_setServerOnNetworkCallback(server(), serverOnNetworkCallback, (void*)(this));
    }
    #endif

    /**
     * start the server
     */
    virtual void start();

    /**
     * stop the server (prior to delete) - do not try start-stop-start
     */
    virtual void stop() {
        _running = false;
    }

    /**
     * Hook called after the server object has been created but before it runs.
     * load configuration files and set up the address space
     * create namespaces and endpoints
     * set up methods and stuff
     */
    virtual void initialise() {}

    /**
     * Hook called between server loop iterations
     * Use it to process thread event
     */
    virtual void process() {}

    /**
     * Hook called before the server is closed
     */
    virtual void terminate();

    /**
     * Get the last execution error code.
     * @return the error code of the last executed function.
     *         UA_STATUSCODE_GOOD (0) if no error.
     */
    UA_StatusCode lastError() const { return _lastError; }

    /**
     * Get the underlying server pointer.
     * @return pointer to underlying server structure
     */
    UA_Server* server() const { return _server; }

    /**
     * Get the running state of the server
     * @return UA_TRUE if the server is running,
     *         UA_FALSE if not yet started or stopping.
     */
    UA_Boolean running() const { return _running; }

    /**
     * Retrieve the context of a given node.
     * @param n node
     * @param[out] pointer to found context of the given node.
     * @return true on success.
     */
    bool getNodeContext(NodeId& n, NodeContext*& c) {
        if (!server()) return false;
        void* p = (void*)(c);
        _lastError = UA_Server_getNodeContext(_server, n.get(), &p);
        return lastOK();
    }

    /**
     * Find a registered node context by its name.
     * @param s name of the context to find.
     * @return a pointer on the found context, nullptr if the context wasn't registered/found.
     */
    static NodeContext* findContext(const std::string& s) {
        return RegisteredNodeContext::findRef(s); // not all node contexts are registered
    }

    /**
     * Assign a given context to a node.
     * @warning The user has to ensure that the destructor call-backs still work.
     * @param n id of the node to modify
     * @param c new context for the modified node.
     *        A context defines the Ctor, Dtor of the node and optionally
     *        the type Ctor and Dtor for object node,
     *        the value read/write methods for variable node,
     *        the data read/write methods for data source node.
     * @return true on success
     */
    bool setNodeContext(NodeId& n, const NodeContext* c) {
        if (!server()) return false;
        _lastError = UA_Server_setNodeContext(_server, n.get(), (void*)(c));
        return lastOK();
    }

    /**
     * Primitive used to retrieve one attribute of a given node, thread-safely.
     * @warning Don't use it directly. Use one of the 19 typed version instead, like readNodeId().
     * There are up to 22 possible node attributes.
     * @param nodeId to read.
     * @param attributeId identify the attribute to retrieve.
     * @param[out] v the retrieved attribute value, must be casted to attribute type.
     *             Some are UA_Boolean, U_NodeId, etc...
     * @see UA_AttributeId for the list of possible attribute id.
     * @return true on success
     */
    bool readAttribute(const UA_NodeId* nodeId, UA_AttributeId attributeId, void* v) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError =  __UA_Server_read(_server, nodeId, attributeId, v);
        return lastOK();
    }

    /**
     * Primitive used to write one attribute of a given node, thread-safely.
     * @warning Don't use it directly. Use one of the 13 typed version instead, like writeValue().
     * There are up to 22 possible node attributes.
     * @param nodeId to write
     * @param attributeId identify the attribute to write. 
     * @see UA_AttributeId for the list of possible attribute id.
     * @param attr_type pointer to the attribute built-in type. Normally stored in the UA_TYPES array.
     * @see UA_TYPES for the list of possible type.
     * @param attr void pointer to the data to write.
     * @return true on success
     */
    bool writeAttribute(
        const UA_NodeId*     nodeId,
        const UA_AttributeId attributeId,
        const UA_DataType*   attr_type,
        const void*          attr) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError = __UA_Server_write(_server, nodeId, attributeId, attr_type, attr);
        return lastOK();
    }

    /**
     * access mutex - most accesses need a write lock
     * @return a reference to the server mutex
     */
    ReadWriteMutex& mutex() { return _mutex; }

    /**
     * Delete a node and all its descendants
     * @param nodeId node to be deleted with its children
     * @return true on success
     */
    bool deleteTree(NodeId& nodeId);

    /**
     * Copy the descendants tree of a given UA_NodeId into a given PropertyTree.
     * Browse the tree from a given UA_NodeId (excluded from the copying)
     * and add all its children as children of the given UANode.
     * @param nodeId parent of the nodes to copy.
     * @param node destination point in tree to which children nodes are added.
     * @return true on success
     */
    bool browseTree(UA_NodeId& nodeId, UANode* node);

    /**
     * Copy the descendants tree of a NodeId into a UANodeTree.
     * Browse the tree from the given NodeId (excluded from the copying)
     * and add all its children as children of the given UANodeTree's root.
     * Produces an addressable tree using dot separated browse path as key.
     * UANodeTree is a specialized PropertyTree using node name as key and NodeId as value.
     * @param nodeId source from which browsing starts in the source tree. It isn't copied, only its children.
     * @param tree the destination UANodeTree. Its root isn't modified.
     * @return true on success.
     */
    bool browseTree(NodeId& nodeId, UANodeTree& tree) {
        return browseTree(nodeId.get(), tree.rootNode());
    }

    /**
     * Copy a NodeId and its descendants tree into a NodeIdMap.
     * NodeIdMap maps a serialized UA_NodeId as key with the UA_NodeId itself as value.
     * @param nodeId the starting point added to the map with its children.
     * @param m the destination NodeIdMap.
     * @return true on success
     */
    bool browseTree(NodeId& nodeId, NodeIdMap& m);

    /**
     * Copy only the non-duplicate children of a UA_NodeId into a NodeIdMap.
     * NodeIdMap maps a serialized UA_NodeId as key with the UA_NodeId itself as value.
     * @param nodeId parent of children to copy
     * @param m map to fill
     * @return true on success
     */
    bool browseChildren(UA_NodeId& nodeId, NodeIdMap& m);

    /**
     * A simplified TranslateBrowsePathsToNodeIds based on the
     * SimpleAttributeOperand type (Part 4, 7.4.4.5).
     * This specifies a relative path using a list of BrowseNames instead of the
     * RelativePath structure. The list of BrowseNames is equivalent to a
     * RelativePath that specifies forward references which are subtypes of the
     * HierarchicalReferences ReferenceType. All Nodes followed by the browsePath
     * shall be of the NodeClass Object or Variable.
     * @param origin the node starting point
     * @param browsePathSize the number of level in the given browse path. Can be less than the actual number of elements.
     * @param browsePath the relative path to browse. Relative to origin.
     * @param[out] result the generated node_id stored in a BrowsePathResult
     * @see BrowsePathResult.
     * @return true on success
     */
    bool browseSimplifiedBrowsePath(
        NodeId              origin,
        size_t              browsePathSize,
        QualifiedName&      browsePath,
        BrowsePathResult&   result) {
        result.get() = UA_Server_browseSimplifiedBrowsePath(
            _server,
            origin,
            browsePathSize,
            browsePath.constRef());
        _lastError = result.ref()->statusCode;
        return lastOK();

    }
    /**
     * create a browse path and add it to the tree
     * @warning not implemented.
     * @todo implement this knockoff of the above browseSimplifiedBrowsePath()
     * @param parent node to start with
     * @param p path to create
     * @param tree
     * @return true on success
     */
    bool createBrowsePath(NodeId& parent, UAPath& p, UANodeTree& tree);

    /**
     * Add a new namespace to the server, thread-safely.
     * @param s name of the new namespace.
     * @return the index of the new namespace.
     */
    UA_UInt16 addNamespace(const std::string s) {
        if (!server()) return 0;

        WriteLock l(mutex());
        return UA_Server_addNamespace(_server, s.c_str());
    }

    /**
     * Get the server configuration.
     * @return a reference to the server configuration as a UA_ServerConfig
     * @warning assumes the configuration is present, undefined behavior otherwise.
     */
    UA_ServerConfig& serverConfig() {
        return* UA_Server_getConfig(server());
    }

    /**
     * Add a new method to the server, thread-safely.
     * @param method point to the method to add.
     * @param browseName method name and description.
     * @param parent parent of the method node
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param nameSpaceIndex of new node, if non-zero otherwise namespace of parent
     * @return true on success
     */
    bool addServerMethod(
        ServerMethod* method,
        const std::string& browseName,
        NodeId& parent,
        NodeId& nodeId,
        NodeId& newNode     = NodeId::Null,
        int nameSpaceIndex  = 0);

    /**
     * Add a new Repeated call-back to the server.
     * @param id name of the call-back used to find it in the call-back map
     * @param p function pointer on the call-back to add.
     */
    void addRepeatedCallback(const std::string& id, SeverRepeatedCallback* p) {
        _callbacks[id] = SeverRepeatedCallbackRef(p);
    }

    /**
     * Create a new Repeated call-back in the server.
     * @param id name of the call-back used to find it in the call-back map
     * @param interval for the call-back periodic call repetition.
     * @param f the call-back
     */
    void addRepeatedCallback(const std::string& id, int interval, SeverRepeatedCallbackFunc f) {
        auto p = new SeverRepeatedCallback(*this, interval, f);
        _callbacks[id] = SeverRepeatedCallbackRef(p);
    }

    /**
     * Remove a repeated call-back from the server.
     * @param id name of the call-back used to find it in the call-back map.
     */
    void removeRepeatedCallback(const std::string& id) {
        _callbacks.erase(id);
    }

    /**
     * Retrieve a repeated call-back from the server.
     * @param s name of the call-back used to find it in the call-back map.
     * @return a reference to the found call-back
     */
    SeverRepeatedCallbackRef& repeatedCallback(const std::string& s) {
        return _callbacks[s];
    }

    /**
     * Get the name and namespace index of a given node
     * @param[in] nodeId of the node to read
     * @param[out] name the qualified name of the node if found
     * @param[out] ns the namespace index of the node if found
     * @return true on success, false otherwise. On failure the output param are of course unchanged.
     */
    bool browseName(NodeId& nodeId, std::string& name, int& ns) {
        if (!_server) throw std::runtime_error("Null server");

        QualifiedName outBrowseName;
        if (UA_Server_readBrowseName(_server, nodeId, outBrowseName) == UA_STATUSCODE_GOOD) {
            name = toString(outBrowseName.get().name);
            ns = outBrowseName.get().namespaceIndex;
        }
        return lastOK();
    }

    /**
     * Set the BrowseName of a node with the given namespace and name, thread-safely.
     * @param nodeId to modify
     * @param nameSpaceIndex part of the new browse name
     * @param name
     */
    void setBrowseName(NodeId& nodeId, int nameSpaceIndex, const std::string& name) {
        if (!server()) return;

        QualifiedName newBrowseName(nameSpaceIndex, name);
        WriteLock l(_mutex);
        UA_Server_writeBrowseName(_server, nodeId, newBrowseName);
    }

    /**
     * Get the node id from the path of browse names in the given namespace. Tests for node existence
     * @param start the reference node for the path
     * @param path relative to start
     * @param nodeId the found node
     * @return true on success, otherwise nodeId refer to the last node matching the path.
     */
    bool nodeIdFromPath(NodeId& start, Path& path,  NodeId& nodeId);

    /**
     * Create folder path first then add variable node to path's end leaf
     * @param start the reference node for the path
     * @param path relative to start
     * @param nameSpaceIndex
     * @param nodeId is a shallow copy - do not delete and is volatile
     * @return true on success
     */
    bool createFolderPath(NodeId& start, Path& path, int nameSpaceIndex, NodeId& nodeId);

    /**
     * Get the child with a specific name of a given node.
     * @param start the parent node
     * @param childName the name of the child node to find.
     * @param[out] the found node.
     * @return true on success
     */
    bool  getChild(NodeId& start, const std::string& childName, NodeId& ret);

    /**
     * Add a children Folder node to a given parent node, thread-safely.
     * @param parent parent node
     * @param childName browse name of the folder node
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param nameSpaceIndex of the new node, if non-zero otherwise namespace of parent
     * @return true on success
     */
    bool addFolder(
        NodeId&             parent,
        const std::string&  childName,
        NodeId&             nodeId,
        NodeId&             newNode         = NodeId::Null,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a new variable node in the server, thread-safely.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param context customize how the node will be created if not null.
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    bool addVariable(
        NodeId&             parent,
        const std::string&  childName,
        Variant&            value,
        NodeId&             nodeId          = NodeId::Null,
        NodeId&             newNode         = NodeId::Null,
        NodeContext*        context         = nullptr,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a variable of the given type, thread-safely.
     * @param T specify the UA_ built-in type.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param context customize how the node will be created if not null.
     * @param newNode receives new node if not null
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    template<typename T>
    bool addVariable(
        NodeId&             parent,
        const std::string&  childName,
        NodeId&             nodeId,
        const std::string&  context,
        NodeId&             newNode         = NodeId::Null,
        int                 nameSpaceIndex  = 0) {
        if (NodeContext* cp = findContext(context)) {
            Variant v(T());
            return addVariable(parent, childName, v, nodeId,  newNode, cp, nameSpaceIndex);
        }
        return false;
    }

    /**
     * Add a new historical variable node in the server, thread-safely.
     * Same as addVariable but with the historizing attribute set to true
     * and the read history access.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param context customize how the node will be created if not null.
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    bool addHistoricalVariable(
        NodeId&             parent,
        const std::string&  childName,
        Variant&            value,
        NodeId&             nodeId          = NodeId::Null,
        NodeId&             newNode         = NodeId::Null,
        NodeContext*        context         = nullptr,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a new historical variable node of the given type in the server, thread-safely.
     * Same as addVariable but with the historizing attribute set to true
     * and the read history access.
     * @param T specify the UA_ built-in type.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param context customize how the node will be created if not null.
     * @param newNode receives new node if not null
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    template<typename T>
    bool addHistoricalVariable(
        NodeId&             parent,
        const std::string&  childName,
        NodeId&             nodeId,
        const std::string&  contextName,
        NodeId&             newNode = NodeId::Null,
        int                 nameSpaceIndex = 0) {
        if (NodeContext* context = findContext(contextName)) {
            Variant v(T());
            return addHistoricalVariable(
                parent, childName, v, nodeId, newNode, context, nameSpaceIndex);
        }
        return false;
    }

    /**
     * Add a new property node in the server, thread-safely.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param context customize how the node will be created if not null.
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    bool addProperty(
        NodeId&             parent,
        const std::string&  childName,
        Variant&            value,
        NodeId&             nodeId          = NodeId::Null,
        NodeId&             newNode         = NodeId::Null,
        NodeContext*        context         = nullptr,
        int                 nameSpaceIndex  = 0);

    /**
     * Add a new property node of the given type in the server, thread-safely.
     * @param T specify the UA_ built-in type.
     * @param parent specify the parent node containing the added node
     * @param childName browse name of the new node
     * @param value variant with the value for the new node. Also specifies its type.
     * @param nodeId assigned node id or NodeId::Null for auto assign
     * @param newNode receives new node if not null
     * @param context customize how the node will be created if not null.
     * @param nameSpaceIndex of the new node if non-zero, otherwise namespace of parent
     * @return true on success
     */
    template <typename T>
    bool addProperty(
        NodeId&             parent,
        const std::string&  childName,
        const T&            value,
        NodeId&             nodeId          = NodeId::Null,
        NodeId&             newNode         = NodeId::Null,
        NodeContext*        context         = nullptr,
        int                 nameSpaceIndex  = 0) {
        Variant v(value);
        return addProperty(
            parent, key, v, nodeId, newNode, context, nameSpaceIndex);
    }
    
    /**
     * deleteNode 
     * @param nodeId 
     * @param deleteReferences 
     * @return true on success 
     */ 
    bool deleteNode(NodeId& nodeId, bool  deleteReferences) {
        if (!server()) return false; 
 
        WriteLock l(_mutex); 
        _lastError =  UA_Server_deleteNode(_server, nodeId, UA_Boolean(deleteReferences)); 
        return _lastError != UA_STATUSCODE_GOOD; 
    } 

    /**
     * call
     * @param request
     * @param ret
     * @return true on success
     */
    bool call(CallMethodRequest& request, CallMethodResult& ret) {
        if (!server()) return false;

        WriteLock l(_mutex);
        ret.get() = UA_Server_call(_server, request);
        return ret.get().statusCode == UA_STATUSCODE_GOOD;
    }

    /**
     * translateBrowsePathToNodeIds
     * @param path
     * @param result
     * @return true on success
     */
    bool translateBrowsePathToNodeIds(BrowsePath& path, BrowsePathResult& result) {
        if (!server()) return false;

        WriteLock l(_mutex);
        result.get() = UA_Server_translateBrowsePathToNodeIds(_server, path);
        return result.get().statusCode  == UA_STATUSCODE_GOOD;
    }

    /**
     * lastOK
     * @return last error code
     */
    bool lastOK() const {
        return _lastError == UA_STATUSCODE_GOOD;
    }

    // Attributes

    /**
     * readNodeId
     * @param nodeId
     * @param outNodeId
     * @return true on success
     */
    bool readNodeId(NodeId& nodeId, NodeId& outNodeId) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_NODEID, outNodeId);
    }

    /**
     * readNodeClass
     * @param nodeId
     * @param outNodeClass
     * @return true on success
     */
    bool readNodeClass(NodeId& nodeId, UA_NodeClass& outNodeClass) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_NODECLASS, &outNodeClass);
    }

    /**
     * readBrowseName
     * @param nodeId
     * @param outBrowseName
     * @return true on success
     */
    bool readBrowseName(NodeId& nodeId, QualifiedName& outBrowseName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME, outBrowseName);
    }

    /**
     * readDisplayName
     * @param nodeId
     * @param outDisplayName
     * @return true on success
     */
    bool readDisplayName(NodeId& nodeId, LocalizedText& outDisplayName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME, outDisplayName);
    }

    /**
     * readDescription
     * @param nodeId
     * @param outDescription
     * @return true on success
     */
    bool readDescription(NodeId& nodeId, LocalizedText& outDescription) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION, outDescription);
    }

    /**
     * readWriteMask
     * @param nodeId
     * @param outWriteMask
     * @return true on success
     */
    bool readWriteMask(NodeId& nodeId, UA_UInt32& outWriteMask) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK, &outWriteMask);
    }

    /**
     * readIsAbstract
     * @param nodeId
     * @param outIsAbstract
     * @return true on success
     */
    bool readIsAbstract(NodeId& nodeId, UA_Boolean& outIsAbstract) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT, &outIsAbstract);
    }

    /**
     * readSymmetric
     * @param nodeId
     * @param outSymmetric
     * @return true on success
     */
    bool readSymmetric(NodeId& nodeId, UA_Boolean& outSymmetric) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_SYMMETRIC, &outSymmetric);
    }

    /**
     * readInverseName
     * @param nodeId
     * @param outInverseName
     * @return true on success
     */
    bool readInverseName(NodeId& nodeId, LocalizedText& outInverseName) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME, outInverseName);
    }

    /**
     * readContainsNoLoop
     * @param nodeId
     * @param outContainsNoLoops
     * @return true on success
     */
    bool readContainsNoLoop(NodeId& nodeId, UA_Boolean& outContainsNoLoops) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_CONTAINSNOLOOPS, &outContainsNoLoops);
    }

    /**
     * readEventNotifier
     * @param nodeId
     * @param outEventNotifier
     * @return 
     */
    bool readEventNotifier(NodeId& nodeId, UA_Byte& outEventNotifier) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EVENTNOTIFIER, &outEventNotifier);
    }

    /**
     * readValue
     * @param nodeId
     * @param outValue
     * @return 
     */
    bool readValue(NodeId& nodeId, Variant& outValue) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUE, outValue);
    }

    /**
     * readDataType
     * @param nodeId
     * @param outDataType
     * @return 
     */
    bool readDataType(NodeId& nodeId, NodeId& outDataType) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE, outDataType);
    }

    /**
     * readValueRank
     * @param nodeId
     * @param outValueRank
     * @return 
     */
    bool readValueRank(NodeId& nodeId, UA_Int32& outValueRank) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK, &outValueRank);
    }

    /* Returns a variant with an int32 array */
    /**
     * readArrayDimensions
     * @param nodeId
     * @param outArrayDimensions
     * @return 
     */
    bool readArrayDimensions(NodeId& nodeId, Variant& outArrayDimensions) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ARRAYDIMENSIONS, outArrayDimensions);
    }

    /**
     * readAccessLevel
     * @param nodeId
     * @param outAccessLevel
     * @return 
     */
    bool readAccessLevel(NodeId& nodeId, UA_Byte& outAccessLevel) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL, &outAccessLevel);
    }

    /**
     * readMinimumSamplingInterval
     * @param nodeId
     * @param outMinimumSamplingInterval
     * @return 
     */
    bool readMinimumSamplingInterval(NodeId& nodeId, UA_Double& outMinimumSamplingInterval) {
        return readAttribute(nodeId,
                              UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                              &outMinimumSamplingInterval);
    }

    /**
     * readHistorizing
     * @param nodeId
     * @param outHistorizing
     * @return 
     */
    bool readHistorizing(NodeId& nodeId, UA_Boolean& outHistorizing) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_HISTORIZING, &outHistorizing);
    }

    /**
     * readExecutable
     * @param nodeId
     * @param outExecutable
     * @return 
     */
    bool readExecutable(NodeId& nodeId, UA_Boolean& outExecutable) {
        return readAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE, &outExecutable);
    }

    /**
     * writeBrowseName
     * @param nodeId
     * @param browseName
     * @return 
     */
    bool writeBrowseName(NodeId& nodeId, QualifiedName& browseName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_BROWSENAME,
                                &UA_TYPES[UA_TYPES_QUALIFIEDNAME], browseName);
    }

    /**
     * writeDisplayName
     * @param nodeId
     * @param displayName
     * @return 
     */
    bool writeDisplayName(NodeId& nodeId, LocalizedText& displayName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DISPLAYNAME,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT], displayName);
    }

    /**
     * writeDescription
     * @param nodeId
     * @param description
     * @return 
     */
    bool writeDescription(NodeId& nodeId, LocalizedText& description) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DESCRIPTION,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT], description);
    }

    /**
     * writeWriteMask
     * @param nodeId
     * @param writeMask
     * @return 
     */
    bool writeWriteMask(NodeId& nodeId, const UA_UInt32 writeMask) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_WRITEMASK,
                                &UA_TYPES[UA_TYPES_UINT32], &writeMask);
    }

    /**
     * writeIsAbstract
     * @param nodeId
     * @param isAbstract
     * @return 
     */
    bool writeIsAbstract(NodeId& nodeId, const UA_Boolean isAbstract) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_ISABSTRACT,
                                &UA_TYPES[UA_TYPES_BOOLEAN], &isAbstract);
    }

    /**
     * writeInverseName
     * @param nodeId
     * @param inverseName
     * @return 
     */
    bool writeInverseName(NodeId& nodeId, const UA_LocalizedText inverseName) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_INVERSENAME,
                                &UA_TYPES[UA_TYPES_LOCALIZEDTEXT], &inverseName);
    }

    /**
     * writeEventNotifier
     * @param nodeId
     * @param eventNotifier
     * @return 
     */
    bool writeEventNotifier(NodeId& nodeId, const UA_Byte eventNotifier) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_EVENTNOTIFIER,
                                &UA_TYPES[UA_TYPES_BYTE], &eventNotifier);
    }

    /**
     * writeValue
     * @param nodeId
     * @param value
     * @return 
     */
    bool writeValue(NodeId& nodeId, Variant& value) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                                &UA_TYPES[UA_TYPES_VARIANT], value);
    }

    /**
     * writeDataType
     * @param nodeId
     * @param dataType
     * @return 
     */
    bool writeDataType(NodeId& nodeId, NodeId& dataType) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_DATATYPE,
                                &UA_TYPES[UA_TYPES_NODEID], dataType);
    }

    /**
     * writeValueRank
     * @param nodeId
     * @param valueRank
     * @return 
     */
    bool writeValueRank(NodeId& nodeId, const UA_Int32 valueRank) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_VALUERANK,
                                &UA_TYPES[UA_TYPES_INT32], &valueRank);
    }

    /**
     * writeArrayDimensions
     * @param nodeId
     * @param arrayDimensions
     * @return 
     */
    bool writeArrayDimensions(NodeId& nodeId, Variant arrayDimensions) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_VALUE,
                                &UA_TYPES[UA_TYPES_VARIANT], arrayDimensions.constRef());
    }

    /**
     * writeAccessLevel
     * @param nodeId
     * @param accessLevel
     * @return 
     */
    bool writeAccessLevel(NodeId& nodeId, const UA_Byte accessLevel) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_ACCESSLEVEL,
                                &UA_TYPES[UA_TYPES_BYTE], &accessLevel);
    }

    // Some short cuts

    /**
     * writeEnable
     * @param nodeId
     * @return 
     */
    bool writeEnable(NodeId& nodeId) {
        UA_Byte accessLevel;
        if (readAccessLevel(nodeId, accessLevel)) {
            accessLevel |= UA_ACCESSLEVELMASK_WRITE;
            return writeAccessLevel(nodeId, accessLevel);
        }
        return false;
    }

    /**
     * setReadOnly
     * @param nodeId
     * @param historyEnable
     * @return 
     */
    bool setReadOnly(NodeId& nodeId, bool historyEnable = false) {
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

    /**
     * writeMinimumSamplingInterval
     * @param nodeId
     * @param miniumSamplingInterval
     * @return 
     */
    bool writeMinimumSamplingInterval(NodeId& nodeId, const UA_Double miniumSamplingInterval) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL,
                                &UA_TYPES[UA_TYPES_DOUBLE], &miniumSamplingInterval);
    }

    /**
     * writeExecutable
     * @param nodeId
     * @param executable
     * @return 
     */
    bool writeExecutable(NodeId& nodeId, const UA_Boolean executable) {
        return writeAttribute(nodeId, UA_ATTRIBUTEID_EXECUTABLE,
                                &UA_TYPES[UA_TYPES_BOOLEAN], &executable);
    }

    // Add Nodes - taken from docs

    /**
     * addVariableNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param typeDefinition
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addVariableNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        NodeId&         typeDefinition,
        VariableAttributes& attr,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    context = nullptr) {
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
            context,
            outNewNodeId.isNull() ? nullptr : outNewNodeId.ref());
        return lastOK();
    }

    /**
     * addVariableTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param typeDefinition
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addVariableTypeNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        NodeId&         typeDefinition,
        VariableTypeAttributes& attr,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError =  UA_Server_addVariableTypeNode(
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

    /**
     * addObjectNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param typeDefinition
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addObjectNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        NodeId&         typeDefinition,
        ObjectAttributes& attr,
        NodeId&         outNewNodeId          = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
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

    /**
     * addObjectTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addObjectTypeNode(
        NodeId&             requestedNewNodeId,
        NodeId&             parentNodeId,
        NodeId&             referenceTypeId,
        QualifiedName&      browseName,
        ObjectTypeAttributes& attr,
        NodeId&             outNewNodeId            = NodeId::Null,
        NodeContext*        instantiationCallback   = nullptr) {
        if (!server()) return false;

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

    /**
     * addViewNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addViewNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        ViewAttributes& attr,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
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

    /**
     * addReferenceTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addReferenceTypeNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        ReferenceTypeAttributes& attr,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
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

    /**
     * addDataTypeNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param attr
     * @param outNewNodeId
     * @param instantiationCallback
     * @return 
     */
    bool addDataTypeNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        DataTypeAttributes& attr,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
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

    /**
     * addDataSourceVariableNode
     * @param requestedNewNodeId
     * @param parentNodeId
     * @param referenceTypeId
     * @param browseName
     * @param typeDefinition
     * @param attr
     * @param dataSource
     * @param outNewNodeId
     * @return 
     */
    bool addDataSourceVariableNode(
        NodeId&         requestedNewNodeId,
        NodeId&         parentNodeId,
        NodeId&         referenceTypeId,
        QualifiedName&  browseName,
        NodeId&         typeDefinition,
        VariableAttributes& attr,
        DataSource&     dataSource,
        NodeId&         outNewNodeId = NodeId::Null,
        NodeContext*    instantiationCallback = nullptr) {
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

    /**
     * addReference
     * @param sourceId
     * @param refTypeId
     * @param targetId
     * @param isForward
     * @return 
     */
    bool addReference(
        NodeId&         sourceId,
        NodeId&         refTypeId,
        ExpandedNodeId& targetId,
        bool            isForward) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError =  UA_Server_addReference(
            server(),
            sourceId,
            refTypeId,
            targetId,
            isForward);
        return lastOK();
    }

    /**
     * markMandatory
     * @param nodeId
     * @return 
     */
    bool markMandatory(NodeId& nodeId) {
        return addReference(
            nodeId,
            NodeId::HasModellingRule,
            ExpandedNodeId::ModellingRuleMandatory,
            true);
    }

    /**
     * deleteReference
     * @param sourceNodeId
     * @param referenceTypeId
     * @param isForward
     * @param targetNodeId
     * @param deleteBidirectional
     * @return 
     */
    bool deleteReference(
        NodeId&         sourceNodeId,
        NodeId&         referenceTypeId,
        bool            isForward,
        ExpandedNodeId& targetNodeId,
        bool            deleteBidirectional) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError =  UA_Server_deleteReference(
            server(),
            sourceNodeId,
            referenceTypeId,
            isForward,
            targetNodeId,
            deleteBidirectional);
        return lastOK();

    }


    /**
     * addInstance
     * @param n
     * @param parent
     * @param nodeId
     * @return 
     */
    bool addInstance(
        const std::string&  n,
        NodeId&             requestedNewNodeId,
        NodeId&             parent,
        NodeId&             typeId,
        NodeId&             nodeId  = NodeId::Null,
        NodeContext*        context = nullptr) {
        if (!server()) return false;

        ObjectAttributes oAttr;
        oAttr.setDefault();
        oAttr.setDisplayName(n);
        
        return addObjectNode(
            requestedNewNodeId,
            parent,
            NodeId::Organizes,
            QualifiedName(parent.nameSpaceIndex(), n),
            typeId,
            oAttr,
            nodeId,
            context);
    }

    /**
     * Creates a node representation of an event
     * @param server The server object
     * @param eventType The type of the event for which a node should be created
     * @param outNodeId The NodeId of the newly created node for the event
     * @return The StatusCode of the UA_Server_createEvent method */
    bool createEvent(const NodeId& eventType, NodeId& outNodeId) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError = UA_Server_createEvent(_server, eventType, outNodeId.ref());
        return lastOK();
    }

    /**
     * Triggers a node representation of an event by applying EventFilters and adding the event to the appropriate queues.
     * @param eventNodeId The NodeId of the node representation of the event which should be triggered
     * @param outEvent the EventId of the new event
     * @param deleteEventNode Specifies whether the node representation of the event should be deleted
     * @return The StatusCode of the UA_Server_triggerEvent method */
    bool triggerEvent(
        NodeId&         eventNodeId,
        UA_ByteString*  outEventId = nullptr,
        bool            deleteEventNode = true) {
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

    /**
     * addNewEventType
     * @param name
     * @param description
     * @param eventType  the event type node
     * @return true on success
     */
    bool addNewEventType(
        const std::string&  name,
        NodeId&             eventType,
        const std::string&  description = std::string()) {
        if (!server()) return false;

        ObjectTypeAttributes attr;
        attr.setDefault();
        attr.setDisplayName(name);
        attr.setDescription((description.empty() ? name : description));

        WriteLock l(_mutex);
        _lastError =  UA_Server_addObjectTypeNode(
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


    /**
     * setUpEvent
     * @param outId
     * @param eventMessage
     * @param eventSourceName
     * @param eventSeverity
     * @param eventTime
     * @return true on success
     */
    bool  setUpEvent(
        NodeId&             outId,
        NodeId&             eventType,
        const std::string&  eventMessage,
        const std::string&  eventSourceName,
        int                 eventSeverity = 100,
        UA_DateTime         eventTime = UA_DateTime_now()) {
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

    /**
     * updateCertificate
     * @param oldCertificate
     * @param newCertificate
     * @param newPrivateKey
     * @param closeSessions
     * @param closeSecureChannels
     * @return true on success
     */
    bool updateCertificate(
        const UA_ByteString* oldCertificate,
        const UA_ByteString* newCertificate,
        const UA_ByteString* newPrivateKey,
        bool                 closeSessions = true,
        bool                 closeSecureChannels = true) {
        if (!server()) return false;

        WriteLock l(_mutex);
        _lastError =  UA_Server_updateCertificate(
            _server,
            oldCertificate,
            newCertificate,
            newPrivateKey,
            closeSessions,
            closeSecureChannels);
        return lastOK();
    }

    /**
     * accessControlAllowHistoryUpdateUpdateData
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param performInsertReplace
     * @param value
     * @return 
     */
    bool accessControlAllowHistoryUpdateUpdateData(
        const NodeId&   sessionId,
        void*           sessionContext,
        const NodeId&   nodeId,
        UA_PerformUpdateType performInsertReplace,
        UA_DataValue&   value) {
        if (!server()) return false;

        WriteLock l(_mutex);
        return UA_Server_AccessControl_allowHistoryUpdateUpdateData(
            _server, sessionId.constRef(),
            sessionContext,
            nodeId.constRef(),
            performInsertReplace,
            &value) == UA_TRUE;
    }

    bool accessControlAllowHistoryUpdateDeleteRawModified(
        const NodeId&   sessionId,
        void*           sessionContext,
        const NodeId&   nodeId,
        UA_DateTime     startTimestamp,
        UA_DateTime     endTimestamp,
        bool            isDeleteModified = true) {
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

    // Access control

    /**
     * allowAddNode
     * @param ac
     * @param sessionId
     * @param sessionContext
     * @param item
     * @return 
     */
    virtual bool allowAddNode(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_AddNodesItem* item) { return true; }

    /**
     * allowAddReference
     * @param ac
     * @param sessionId
     * @param sessionContext
     * @param item
     * @return 
     */
    virtual bool allowAddReference(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_AddReferencesItem* item) { return true; }

    /**
     * allowDeleteNode
     * @param ac
     * @param sessionId
     * @param sessionContext
     * @param item
     * @return 
     */
    virtual bool allowDeleteNode(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_DeleteNodesItem* item) { return false; } // Do not allow deletion from client


    /**
     * allowDeleteReference
     * @param ac
     * @param sessionId
     * @param sessionContext
     * @param item
     * @return 
     */
    virtual bool allowDeleteReference(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_DeleteReferencesItem* item) { return true; }

    /**
     * activateSession
     * @return 
     */
    virtual UA_StatusCode activateSession(
        UA_AccessControl*           ac,
        const UA_EndpointDescription* endpointDescription,
        const UA_ByteString*        secureChannelRemoteCertificate,
        const UA_NodeId*            sessionId,
        const UA_ExtensionObject*   userIdentityToken,
        void**                      sessionContext)  { return UA_STATUSCODE_BADSESSIONIDINVALID; }

    /**
     * De-authenticate a session and cleanup
     */
    virtual void closeSession(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext)   {}

    /**
     * Access control for all nodes
     */
    virtual uint32_t getUserRightsMask(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        void*               nodeContext) { return 0; }

    /**
     * Additional access control for variable nodes
     */
    virtual uint8_t getUserAccessLevel(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        void*               nodeContext) { return 0; }

    /**
    * Additional access control for method nodes
    */
    virtual bool getUserExecutable(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    methodId,
        void*               methodContext) { return false; }

    /** 
     * Additional access control for calling a method node
     * in the context of a specific object
     */
    virtual bool getUserExecutableOnObject(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    methodId,
        void*               methodContext,
        const UA_NodeId*    objectId,
        void*               objectContext) { return false; }
    
    /**
     * Allow insert, replace, update of historical data
     */
    virtual bool allowHistoryUpdateUpdateData(
        UA_AccessControl*       ac,
        const UA_NodeId*        sessionId,
        void*                   sessionContext,
        const UA_NodeId*        nodeId,
        UA_PerformUpdateType    performInsertReplace,
        const UA_DataValue*     value) { return false; }

    /**
     *Allow delete of historical data
     */
    virtual bool allowHistoryUpdateDeleteRawModified(
        UA_AccessControl*   ac,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_DateTime         startTimestamp,
        UA_DateTime         endTimestamp,
        bool                isDeleteModified) { return false; }

    /**
     * setHistoryDatabase
     * Publish - Subscribe interface
     * @param h
     */
    void setHistoryDatabase(UA_HistoryDatabase& h) {
        if (_config) _config->historyDatabase = h;
    }
};

} // namespace open62541

#endif // OPEN62541SERVER_H
