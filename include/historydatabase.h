#ifndef HISTORYDATABASE_H
#define HISTORYDATABASE_H
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "open62541.h"
#include "open62541objects.h"
#ifndef OPEN62541SERVER_H
#include "open62541server.h"
#endif

namespace Open62541 {

class Server;

/**
 * The HistoryDataGathering class
 * Wrap the Historian classes in C++
 * probably the memory database will be all that is needed most of the time
 */
class HistoryDataGathering {
public:
    struct Context {
        Server& server;
        NodeId  sessionId;
        void*   sessionContext = nullptr;
        NodeId  nodeId;

        /**
         * HistoryDataGathering::Context::Context
         * wrap the standard arg items into a single struct to make life easier
         * @param s
         * @param nId
         */
        Context(UA_Server* pServer, const UA_NodeId* pNode = nullptr)
            : server(*Server::findServer(pServer))
            , nodeId(*pNode) {}

    }; // HistoryDataGathering::Context struct

private:
    UA_HistoryDataGathering _gathering;

    // Static callbacks
    static void _deleteMembers(UA_HistoryDataGathering* gathering);

    /**
     * This function registers a node for the gathering of historical data.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param nodeId is the id of the node to register.
     * @param setting contains the gathering settings for the node to register.
     */
    static UA_StatusCode _registerNodeId(
        UA_Server*                          server,
        void*                               hdgContext,
        const UA_NodeId*                    nodeId,
        const UA_HistorizingNodeIdSettings  setting);

    /**
     * This function stops polling a node for value changes.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param nodeId is the id of the node for which polling shall be stopped.
     * @param setting contains the gathering settings for the node.
     */
    static UA_StatusCode _stopPoll(
        UA_Server*          server,
        void*               hdgContext,
        const UA_NodeId*    nodeId);

    /**
     * This function starts polling a node for value changes.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param nodeId is the id of the node for which polling shall be started.
     */
    static UA_StatusCode _startPoll(
        UA_Server*       server,
        void*            hdgContext,
        const UA_NodeId* nodeId);

    /**
     * This function modifies the gathering settings for a node.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param nodeId is the id of the node for which gathering shall be modified.
     * @param setting contains the new gathering settings for the node.
     */
    static UA_Boolean _updateNodeIdSetting(
        UA_Server*                          server,
        void*                               hdgContext,
        const UA_NodeId*                    nodeId,
        const UA_HistorizingNodeIdSettings  setting);

    /**
     * Returns the gathering settings for a node.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param nodeId is the id of the node for which the gathering settings shall be retrieved.
     */
    static const UA_HistorizingNodeIdSettings* _getHistorizingSetting(
        UA_Server*          server,
        void*               hdgContext,
        const UA_NodeId*    nodeId);

    /** Sets a DataValue for a node in the historical data storage.
     * @param server is the server the node lives in.
     * @param hdgContext is the context of the UA_HistoryDataGathering.
     * @param sessionId and sessionContext identify the session which wants to set this value.
     * @param nodeId is the id of the node for which a value shall be set.
     * @param historizing is the historizing flag of the node identified by nodeId.
     * @param value is the value to set in the history data storage.
     */
    static void _setValue(
        UA_Server*          server,
        void*               hdgContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_Boolean          historizing,
        const UA_DataValue* value);

public:
    /**
     * HistoryDataGathering
     * @param initialNodeIdStoreSize
     */
    HistoryDataGathering() = default;

    virtual ~HistoryDataGathering() { deleteMembers(); }

    /**
     * setDefault
     * map to default historian memory
     * @param initialNodeIdStoreSize
     */
    void setDefault(size_t initialNodeIdStoreSize = 100) {
        _gathering = UA_HistoryDataGathering_Default(initialNodeIdStoreSize);
    }

    /**
     * initialise
     * map to class methods
     */
    void initialise();

    /**
     * gathering
     * @return 
     */
    UA_HistoryDataGathering& gathering() { return _gathering; }

    /**
     * deleteMembers
     */
    virtual void deleteMembers() {}

    /**
     * This function registers a node for the gathering of historical data.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the gathering settings for the node to register.
     */
    virtual UA_StatusCode registerNodeId(Context& context, UA_HistorizingNodeIdSettings setting) {
        return 0;
    }

    /**
     * This function stops polling a node for value changes.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the gathering settings for the node.
     */
    virtual UA_StatusCode stopPoll(Context& context) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function starts polling a node for value changes.
     * @param context is the context of the UA_HistoryDataGathering.
     */
    virtual UA_StatusCode startPoll(Context& context) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function modifies the gathering settings for a node.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the new gathering settings for the node.
     */
    virtual UA_Boolean updateNodeIdSetting(Context& context, UA_HistorizingNodeIdSettings setting) {
        return UA_FALSE;
    }

    /**
     * Returns the gathering settings for a node.
     * @param context is the context of the UA_HistoryDataGathering.
     */
    virtual const UA_HistorizingNodeIdSettings* getHistorizingSetting(Context& context) {
        return nullptr;
    }

    /**
     * Sets a DataValue for a node in the historical data storage.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param historizing is the historizing flag of the node identified by nodeId.
     * @param value is the value to set in the history data storage.
     */
    virtual void setValue(
        Context&            context,
        UA_Boolean          historizing,
        const UA_DataValue* value) {}
};

/**
 * The HistoryDatabase class
 * This is the historian storage database
 */
class HistoryDataBackend {
public:
    struct Context {
        Server& server;
        NodeId  sessionId;
        void*   sessionContext;
        NodeId  nodeId;

        /**
         * Call back context common to most call backs.
         * move common bits into one structure so we can simplify calls and maybe do extra magic
         * @param s
         * @param sId
         * @param sContext
         * @param nId
         */
        Context(
            UA_Server*       pServer,
            const UA_NodeId* pNodeSession,
            void*            pSessionContext,
            const UA_NodeId* pNode)
            : server(*Server::findServer(pServer))
            , sessionId(*pNodeSession)
            , sessionContext(pSessionContext)
            , nodeId(*pNode) {}
    }; // HistoryDataBackend::Context class

private:
    UA_HistoryDataBackend _database; /**< the database structure */

    // Define the callbacks
    static void _deleteMembers(UA_HistoryDataBackend* backend);

    /**
     * _serverSetHistoryData
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param historizing
     * @param value
     * @return 
     */
    static UA_StatusCode _serverSetHistoryData(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_Boolean          historizing,
        const UA_DataValue* value);

    static  UA_StatusCode _getHistoryData(
        UA_Server*              server,
        const UA_NodeId*        sessionId,
        void*                   sessionContext,
        const UA_HistoryDataBackend* backend,
        const UA_DateTime       start,
        const UA_DateTime       end,
        const UA_NodeId*        nodeId,
        size_t                  maxSizePerResponse,
        UA_UInt32               numValuesPerNode,
        UA_Boolean              returnBounds,
        UA_TimestampsToReturn   timestampsToReturn,
        UA_NumericRange         range,
        UA_Boolean              releaseContinuationPoints,
        const UA_ByteString*    continuationPoint,
        UA_ByteString*          outContinuationPoint,
        UA_HistoryData*         result);

    /**
     * _getDateTimeMatch
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param timestamp
     * @param strategy
     * @return 
     */
    static size_t _getDateTimeMatch(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DateTime   timestamp,
        const MatchStrategy strategy);

    /**
     * _getEnd
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @return 
     */
    static size_t _getEnd(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * _lastIndex
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @return 
     */
    static size_t _lastIndex(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * _firstIndex
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @return 
     */
    static size_t _firstIndex(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * _resultSize
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param startIndex
     * @param endIndex
     * @return 
     */
    static size_t _resultSize(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        size_t              startIndex,
        size_t              endIndex);

    /**
     * _copyDataValues
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param startIndex
     * @param endIndex
     * @param reverse
     * @param valueSize
     * @param range
     * @param releaseContinuationPoints
     * @param continuationPoint
     * @param outContinuationPoint
     * @param providedValues
     * @param values
     * @return 
     */
    static UA_StatusCode _copyDataValues(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        size_t              startIndex,
        size_t              endIndex,
        UA_Boolean          reverse,
        size_t              valueSize,
        UA_NumericRange     range,
        UA_Boolean          releaseContinuationPoints,
        const UA_ByteString* continuationPoint,
        UA_ByteString*      outContinuationPoint,
        size_t*             providedValues,
        UA_DataValue*       values);

    /**
     * _getDataValue
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param index
     * @return 
     */
    static const UA_DataValue* _getDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        size_t              index);


    static UA_Boolean _boundSupported(
        UA_Server*       server,
        void*            hdbContext,
        const UA_NodeId* sessionId,
        void*            sessionContext,
        const UA_NodeId* nodeId);


    static UA_Boolean _timestampsToReturnSupported(
        UA_Server*                  server,
        void*                       hdbContext,
        const UA_NodeId*            sessionId,
        void*                       sessionContext,
        const UA_NodeId*            nodeId,
        const UA_TimestampsToReturn timestampsToReturn);


    static UA_StatusCode _insertDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    static  UA_StatusCode _replaceDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    static UA_StatusCode _updateDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    /**
     * _removeDataValue
     * @param server
     * @param hdbContext
     * @param sessionId
     * @param sessionContext
     * @param nodeId
     * @param startTimestamp
     * @param endTimestamp
     * @return 
     */
    static UA_StatusCode _removeDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_DateTime         startTimestamp,
        UA_DateTime         endTimestamp);

public:
    HistoryDataBackend() {
        memset(&_database, 0, sizeof(_database));
    }

    void setMemory(size_t nodes = 100, size_t size = 1000000) {
        _database = UA_HistoryDataBackend_Memory(nodes, size);
    }

    /**
     * initialise to use class methods
     */
    void initialise();

    /**
     * ~HistoryDatabase
     */
    virtual ~HistoryDataBackend() { deleteMembers(); }

    UA_HistoryDataBackend& database() { return _database; }

    /**
     * deleteMembers
     */
    virtual void deleteMembers() {

    }

    /**
     * This function sets a DataValue for a node in the historical data storage.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param historizing is the historizing flag of the node identified by nodeId.
     *        If sessionId is NULL, the historizing flag is invalid and must not be used.
     * @param value is the value which shall be stored.
     */
    virtual  UA_StatusCode serverSetHistoryData(
        Context&            context,
        bool                historizing,
        const UA_DataValue* value) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function is the high level interface for the ReadRaw operation. Set
     * it to NULL if you use the low level API for your plugin. It should be
     * used if the low level interface does not suite your database. It is more
     * complex to implement the high level interface but it also provide more
     * freedom. If you implement this, then set all low level API function
     * pointer to NULL.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param start is the start time of the HistoryRead request.
     * @param end is the end time of the HistoryRead request.
     * @param nodeId is the id of the node for which historical data is requested.
     * @param maxSizePerResponse is the maximum number of items per response the server can provide.
     * @param numValuesPerNode is the maximum number of items per response the client wants to receive.
     * @param returnBounds determines if the client wants to receive bounding values.
     * @param timestampsToReturn contains the time stamps the client is interested in.
     * @param range is the numeric range the client wants to read.
     * @param releaseContinuationPoints determines if the continuation points shall be released.
     * @param continuationPoint is the continuation point the client wants to release or start from.
     * @param outContinuationPoint is the continuation point that gets passed to the
     *        client by the HistoryRead service.
     * @param result contains the result history data that gets passed to the client.
     */
    virtual UA_StatusCode getHistoryData(
        Context&            context,
        const UA_DateTime   start,
        const UA_DateTime   end,
        size_t              maxSizePerResponse,
        UA_UInt32           numValuesPerNode,
        UA_Boolean          returnBounds,
        UA_TimestampsToReturn timestampsToReturn,
        UA_NumericRange     range,
        UA_Boolean          releaseContinuationPoints,
        std::string&        continuationPoint,
        std::string&        outContinuationPoint,
        UA_HistoryData*     result) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function is part of the low level HistoryRead API. It returns the
     * index of a value in the database which matches certain criteria.
     * 
     * server is the server the node lives in.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param timestamp is the timestamp of the requested index.
     * @param strategy is the matching strategy which shall be applied in finding the index.
     */
    virtual size_t getDateTimeMatch(
        Context&            context,
        const UA_DateTime   timestamp,
        const MatchStrategy strategy) {
        return 0;
    }

    /**
     * This function is part of the low level HistoryRead API.
     * It returns the index of the element after the last valid entry 
     * in the database for a node.
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId and sessionContext identify the session that wants to read historical data.
     * @param nodeId is the id of the node for which the end of storage shall be returned.
     */
    virtual size_t getEnd(Context& hdbContext) {
        return 0;
    }
    
    /**
     * This function is part of the low level HistoryRead API.
     * It returns the index of the last element in the database for a node.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @return the index of the last element in the database for a node.
     */
    virtual size_t lastIndex(Context& hdbContext) {
        return 0;
    }

    /**
     * This function is part of the low level HistoryRead API.
     * It returns the index of the first element in the database for a node.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @return the index of the first element in the database for a node.
     */
    virtual size_t firstIndex(Context& hdbContext) {
        return 0;
    }
    
    /**
     * This function is part of the low level HistoryRead API.
     * It returns the number of elements between startIndex and endIndex including both.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param startIndex is the index of the first element in the range.
     * @param endIndex is the index of the last element in the range.
     * @return the number of elements between startIndex and endIndex including both.
     */
    virtual size_t resultSize(Context& hdbContext, size_t startIndex, size_t endIndex) {
        return 0;
    }

    /**
     * This function is part of the low level HistoryRead API.
     * It copies data values inside a certain range into a buffer.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param startIndex is the index of the first value in the range.
     * @param endIndex is the index of the last value in the range.
     * @param reverse determines if the values shall be copied in reverse order.
     * @param valueSize is the maximal number of data values to copy.
     * @param range is the numeric range which shall be copied for every data value.
     * @param releaseContinuationPoints determines if the continuation points shall be released.
     * @param continuationPoint is a continuation point the client wants to release or start from.
     * @param outContinuationPoint is a continuation point which will be passed to the client.
     * @param providedValues contains the number of values that were copied.
     * @param values contains the values that have been copied from the database.
     */
    virtual UA_StatusCode copyDataValues(
        Context&        context,
        size_t          startIndex,
        size_t          endIndex,
        UA_Boolean      reverse,
        size_t          valueSize,
        UA_NumericRange range,
        UA_Boolean      releaseContinuationPoints,
        std::string&    in,
        std::string&    out,
        size_t*         providedValues,
        UA_DataValue*   values) {
        return 0;
    }

    /**
     * This function is part of the low level HistoryRead API.
     * It returns the data value stored at a certain index in the database.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param index is the index in the database for which the data value is requested. */
    virtual const UA_DataValue* getDataValue(Context& context, size_t index) {
        return nullptr;
    }

    /**
     * This function returns UA_TRUE if the backend supports returning bounding
     * values for a node. This function is mandatory.
     * @param context is the context of the UA_HistoryDataBackend.
     * @return UA_TRUE if the backend supports returning bounding, UA_FALSE otherwise.
     */
    virtual UA_Boolean boundSupported(Context& context) {
        return UA_FALSE;
    }

    /**
     * This function returns UA_TRUE if the backend supports returning the
     * requested timestamps for a node. This function is mandatory.
     * @param context is the context of the UA_HistoryDataBackend.
     * @param sessionId and sessionContext identify the session that wants to read historical data.
     * @param nodeId is the id of the node for which the capability
     *        to return certain timestamps shall be queried.
     */
    virtual UA_Boolean timestampsToReturnSupported(Context& context, UA_TimestampsToReturn timestampsToReturn) {
        return UA_FALSE;
    }

    /**
     * insertDataValue
     * @return 
     */
    virtual UA_StatusCode insertDataValue(Context& context, const UA_DataValue* value) {
        return 0;
    }

    /**
     * replaceDataValue
     * @return 
     */
    virtual UA_StatusCode replaceDataValue(Context& context, const UA_DataValue* value) {
        return 0;
    }

    /**
     * updateDataValue
     * @return 
     */
    virtual UA_StatusCode updateDataValue(Context& context, const UA_DataValue* value) {
        return 0;
    }

    /**
     * removeDataValue
     * @return 
     */
    virtual UA_StatusCode removeDataValue(
        Context&    context,
        UA_DateTime startTimestamp,
        UA_DateTime endTimestamp) {
        return 0;
    }
};

class HistoryDatabase {

    struct Context {
        Server& server;
        NodeId  sessionId;
        void*   sessionContext;
        NodeId  nodeId;
        
        /**
         * HistoryDatabase::Context
         * @param s
         * @param sId
         * @param sContext
         * @param nId
         */
        Context(
            UA_Server*          pServer,
            const UA_NodeId*    pSessionNode,
            void*               pSessionContext,
            const UA_NodeId*    pNode)
            : server(*Server::findServer(pServer))
            , sessionId(*pSessionNode)
            , sessionContext(pSessionContext)
            , nodeId(*pNode) {}
    }; // HistoryDatabase::Context class

    UA_HistoryDatabase _database;

    static void _deleteMembers(UA_HistoryDatabase* hdb) {
        if (hdb && hdb->context) {
            auto p = static_cast<HistoryDatabase*>(hdb->context);
            p->deleteMembers();
        }
    }

    /**
     * Hook called when a nodes value is set.
     * Use this to insert data into your database(s) if polling is not suitable
     * and you need to get all data changes.
     * Set it to NULL if you do not need it.
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId and sessionContext identify the session which set this value.
     * @param nodeId is the node id for which data was set.
     * @param historizing is the nodes boolean flag for historizing
     * @param value is the new value.
     */
    static void _setValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_Boolean          historizing,
        const UA_DataValue* value);

    /**
     * This function is called if a history read is requested with
     * isRawReadModified set to false. Setting it to NULL will result in a
     * response with status code UA_STATUSCODE_BADHISTORYOPERATIONUNSUPPORTED.
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId and sessionContext identify the session which set this value.
     * @param requestHeader, historyReadDetails, timestampsToReturn, releaseContinuationPoints
     * @param nodesToReadSize and nodesToRead is the requested data from the client. It
     *        is from the request object.
     * @param response the response to fill for the client. If the request is ok, there
     *        is no need to use it. Use this to set status codes other than
     *        "Good" or other data. You find an already allocated
     *        UA_HistoryReadResult array with an UA_HistoryData object in the
     *        extension object in the size of nodesToReadSize. If you are not
     *        willing to return data, you have to delete the results array,
     *        set it to NULL and set the resultsSize to 0. Do not access
     *        historyData after that.
     * @param historyData is a proper typed pointer array pointing in the
     *        UA_HistoryReadResult extension object. use this to provide
     *        result data to the client. Index in the array is the same as
     *        in nodesToRead and the UA_HistoryReadResult array.
     */
    static void _readRaw(
        UA_Server*                      server,
        void*                           hdbContext,
        const UA_NodeId*                sessionId,
        void*                           sessionContext,
        const UA_RequestHeader*         requestHeader,
        const UA_ReadRawModifiedDetails* historyReadDetails,
        UA_TimestampsToReturn           timestampsToReturn,
        UA_Boolean                      releaseContinuationPoints,
        size_t                          nodesToReadSize,
        const UA_HistoryReadValueId*    nodesToRead,
        UA_HistoryReadResponse*         response,
        UA_HistoryData* const* const    historyData);

    static void _updateData(
        UA_Server*                  server,
        void*                       hdbContext,
        const UA_NodeId*            sessionId,
        void*                       sessionContext,
        const UA_RequestHeader*     requestHeader,
        const UA_UpdateDataDetails* details,
        UA_HistoryUpdateResult*     result);

    static void _deleteRawModified(
        UA_Server*              server,
        void*                   hdbContext,
        const UA_NodeId*        sessionId,
        void*                   sessionContext,
        const UA_RequestHeader* requestHeader,
        const UA_DeleteRawModifiedDetails* details,
        UA_HistoryUpdateResult* result);


public:
    HistoryDatabase()               = default;
    virtual ~HistoryDatabase()      = default;

    UA_HistoryDatabase& database()  { return _database; }
    virtual void deleteMembers()    {}

    /**
     * This function will be called when a nodes value is set.
     * Use this to insert data into your database(s) if polling is not suitable
     * and you need to get all data changes.
     * Set it to NULL if you do not need it.
     * @param context is the context of the UA_HistoryDatabase.
     * @param historizing is the nodes boolean flag for historizing
     * @param value is the new value.
     */
    virtual void setValue(
        Context &           context,
        UA_Boolean          historizing,
        const UA_DataValue* value)  {}

    /**
     * This function is called if a history read is requested with
     * isRawReadModified set to false. Setting it to NULL will result in a
     * response with status code UA_STATUSCODE_BADHISTORYOPERATIONUNSUPPORTED.
     *
     * @param context is the context of the UA_HistoryDatabase.
     * @param requestHeader, historyReadDetails, timestampsToReturn, releaseContinuationPoints
     * @param nodesToReadSize and nodesToRead is the requested data from the client. It
     *        is from the request object.
     * @param response the response to fill for the client. If the request is ok, there
     *        is no need to use it. Use this to set status codes other than
     *        "Good" or other data. You find an already allocated
     *        UA_HistoryReadResult array with an UA_HistoryData object in the
     *        extension object in the size of nodesToReadSize. If you are not
     *        willing to return data, you have to delete the results array,
     *        set it to NULL and set the resultsSize to 0. Do not access
     *        historyData after that.
     * @param historyData is a proper typed pointer array pointing in the
     *        UA_HistoryReadResult extension object. use this to provide
     *        result data to the client. Index in the array is the same as
     *        in nodesToRead and the UA_HistoryReadResult array.
     */
    virtual void readRaw(
        Context&                        context,
        const UA_RequestHeader*         requestHeader,
        const UA_ReadRawModifiedDetails* historyReadDetails,
        UA_TimestampsToReturn           timestampsToReturn,
        UA_Boolean                      releaseContinuationPoints,
        size_t                          nodesToReadSize,
        const UA_HistoryReadValueId*    nodesToRead,
        UA_HistoryReadResponse*         response,
        UA_HistoryData* const* const    historyData) {
    }

    virtual void updateData(
        Context&                    context,
        const UA_RequestHeader*     requestHeader,
        const UA_UpdateDataDetails* details,
        UA_HistoryUpdateResult*     result) {
    }

    virtual void deleteRawModified(
        Context&                    context,
        const UA_RequestHeader*     requestHeader,
        const UA_DeleteRawModifiedDetails* details,
        UA_HistoryUpdateResult*     result) {
    }

    /*  Add more function pointer here.
        For example for read_event, read_modified, read_processed, read_at_time */
};

/**
 * The Historian class
 * Base class - the C++ abstractions shallow copy the database, backend and gathering structs
 * The C++ abstractions need to have a life time longer than the server
 * This aggregation is used to set the historian on nodes
 */
class Historian {
protected:
    // the parts
    UA_HistoryDatabase      _database;
    UA_HistoryDataBackend   _backend;
    UA_HistoryDataGathering _gathering;

public:
    Historian();

    virtual ~Historian();

    // accessors
    UA_HistoryDatabase&         database()  { return _database; }
    UA_HistoryDataGathering&    gathering() { return _gathering; }
    UA_HistoryDataBackend&      backend()   { return _backend; }

    /**
     * setUpdateNode
     * @param nodeId
     * @param server
     * @param responseSize
     * @param context
     * @return true on success
     */
    bool setUpdateNode(
        NodeId& nodeId,
        Server& server,
        size_t  responseSize = 100,
        size_t  pollInterval = 1000,
        void*   context      = nullptr);
    
    /**
     * setPollNode
     * @param nodeId
     * @param server
     * @param responseSize
     * @param pollInterval
     * @param context
     * @return true on success
     */
    bool setPollNode(
        NodeId& nodeId,
        Server& server,
        size_t  responseSize = 100,
        size_t  pollInterval = 1000,
        void*   context      = nullptr);
    
    /**
     * Historian::setUserNode
     * @param nodeId
     * @param server
     * @param context
     * @return true on success
     */
    bool setUserNode(
        NodeId& nodeId,
        Server& server,
        size_t  responseSize = 100,
        size_t  pollInterval = 1000,
        void*   context      = nullptr);
};

/**
 * The MemoryHistorian class
 * This is the provided in memory historian
 */
class MemoryHistorian : public Historian {
public:
    MemoryHistorian(size_t numberNodes = 100, size_t maxValuesPerNode = 100);
    ~MemoryHistorian() = default;
};

} // namespace Open62541

#endif // HISTORYDATABASE_H
