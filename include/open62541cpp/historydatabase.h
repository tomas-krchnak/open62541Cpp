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

#ifndef OPEN62541SERVER_H
#include <open62541cpp/open62541server.h>
#endif
#ifndef OPEN62541OBJECTS_H
#include <open62541cpp/open62541objects.h>
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
    /**
     * Helper struct aggregating common call-backs arguments
     */
    struct Context {
        Server& server;
        NodeId  sessionId;
        void*   sessionContext = nullptr;
        NodeId  nodeId;

        /**
         * Constructor with empty session.
         * @param server of the historian node.
         * @param pNode historian node.
         */
        Context(UA_Server* pServer, const UA_NodeId* pNode = nullptr)
            : server(*Server::findServer(pServer))
            , nodeId(*pNode) {}

    }; // HistoryDataGathering::Context struct

private:
    UA_HistoryDataGathering m_gathering;

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
     * @param initialStoreSize
     */
    void setDefault(size_t initialStoreSize = 100) {
        m_gathering = UA_HistoryDataGathering_Default(initialStoreSize);
    }

    /**
     * initialise all members of _gathering. Map call-backs to class methods
     */
    void initialise();

    /**
     * @return a reference to the underlying UA_HistoryDataGathering
     */
    UA_HistoryDataGathering& gathering() { return m_gathering; }

    /**
     * Hook called during destruction, permitting to customized the destructor.
     * @Warning Must not throw exception.
     */
    virtual void deleteMembers() {}

    /**
     * This function registers a node for the gathering of historical data.
     * Hook customizing _registerNodeId() call-back.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the gathering settings for the node to register.
     */
    virtual UA_StatusCode registerNodeId(Context& context, UA_HistorizingNodeIdSettings setting) {
        return 0;
    }

    /**
     * This function stops polling a node for value changes.
     * Hook customizing _stopPoll() call-back.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the gathering settings for the node.
     */
    virtual UA_StatusCode stopPoll(Context& context) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function starts polling a node for value changes.
     * Hook customizing _startPoll() call-back.
     * @param context is the context of the UA_HistoryDataGathering.
     */
    virtual UA_StatusCode startPoll(Context& context) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * This function modifies the gathering settings for a node.
     * Hook customizing _updateNodeIdSetting() call-back.
     * @param context is the context of the UA_HistoryDataGathering.
     * @param setting contains the new gathering settings for the node.
     */
    virtual UA_Boolean updateNodeIdSetting(Context& context, UA_HistorizingNodeIdSettings setting) {
        return UA_FALSE;
    }

    /**
     * Returns the gathering settings for a node.
     * Hook customizing _getHistorizingSetting() call-back.
     * @param context is the context of the UA_HistoryDataGathering.
     */
    virtual const UA_HistorizingNodeIdSettings* getHistorizingSetting(Context& context) {
        return nullptr;
    }

        /**
     * Hook called when a node's Data Value is set.
     * Hook customizing _setValue() call-back. 
     * Sets a DataValue for a node in the historical data storage.
     * Use this to insert data into your database(s) if polling is not suitable
     * and you need to get all data changes.
     * Set it to NULL if you do not need it.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDatabase.
     * @param historizing is the nodes boolean flag for historizing
     * @param value is the new value.
     */
    virtual void setValue(
        Context&            context,
        UA_Boolean          historizing,
        const UA_DataValue* value) {}
};

/**
 * The HistoryDataBackend class
 * This is the historian storage database
 */
class HistoryDataBackend {
public:
    /**
    * Helper struct aggregating common call-backs arguments.
    */
    struct Context {
        Server& server;
        NodeId  sessionId;
        void*   sessionContext;
        NodeId  nodeId;

        /**
         * Call back context common to most call backs.
         * move common bits into one structure so we can simplify calls and maybe do extra magic
         * @param pServer
         * @param pSessionId
         * @param pSessionContext
         * @param pNode
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
    UA_HistoryDataBackend m_database; /**< the database structure */

    // Define the callbacks
    static void _deleteMembers(UA_HistoryDataBackend* backend);

    /**
     * Call-back setting a DataValue for a node in the historical data storage.
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId and sessionContext identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId is the node for which the value shall be stored.
     * @param historizing is the historizing flag of the node identified by nodeId.
     * @param value is the value which shall be stored.
     * If sessionId is NULL, the historizing flag is invalid and must not be used.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static UA_StatusCode _serverSetHistoryData(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        UA_Boolean          historizing,
        const UA_DataValue* value);

    /**
     * Call-back returning the history data of a given node.
     * It is the high level interface for the ReadRaw operation.
     * Set it to NULL if you use the low level API for your plug-in.
     * It should be used if the low level interface does not suite your database.
     * It is more complex to implement the high level interface but it also provide more freedom.
     * If you implement it, then set all low level API function pointer to NULL.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param backend is the HistoryDataBackend whose storage is to be queried.
     * @param start is the start time of the HistoryRead request.
     * @param end is the end time of the HistoryRead request.
     * @param nodeId id of the node for which historical data is requested.
     * @param maxSizePerResponse is the maximum number of items per response
     *        the server can provide.
     * @param numValuesPerNode is the maximum number of items per response
     *        the client wants to receive.
     * @param returnBounds determines if the client wants to receive bounding values.
     * @param timestampsToReturn contains the time stamps the client is interested in.
     * @param range is the numeric range the client wants to read.
     * @param releaseContinuationPoints determines if the continuation points
     *        shall be released.
     * @param continuationPoint is the continuation point the client wants to release
     *        or start from.
     * @param outContinuationPoint is the continuation point that gets passed to the
     *        client by the HistoryRead service.
     * @param result contains the result history data that gets passed to the client.
     * @return UA_STATUSCODE_GOOD on success.
    */
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
     * Call-back returning the index of a value in the database matching given criteria.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the matching value shall be found.
     * @param timestamp is the timestamp of the requested index.
     * @param strategy is the matching strategy which shall be applied in finding the index.
     * @return the index of the value matching the strategy's criteria.
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
     * Call-back returning the index of the element after the last valid entry
     * in the database of given node.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the end of storage shall be returned.
     * @return the index of the element after the last valid entry in the database
     *         of the given node.
     */
    static size_t _getEnd(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * Call-back returning the index of the last element in the database for a node.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the index of the last element
     *        shall be returned.
     * @return the index of the last element in the database for the node
     */
    static size_t _lastIndex(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * Call-back returning the index of the first element in the database for a node.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the index of the first
     *        element shall be returned.
     * @return the index of the first element in the database for the node.
     */
    static size_t _firstIndex(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId);

    /**
     * Call-back returning the number of elements between startIndex and endIndex including both.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the number of elements shall be returned.
     * @param startIndex is the index of the first element in the range.
     * @param endIndex is the index of the last element in the range.
     * @return the number of elements between startIndex and endIndex including both.
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
     * Call-back copying data values inside a certain range into a buffer.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the data values shall be copied.
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
     * @return UA_STATUSCODE_GOOD on success.
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
     * Call-back returning the data value stored at a certain index in the database.
     * It is part of the low level HistoryRead API.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the data value shall be returned.
     * @param index is the index in the database for which the data value is requested.
     * @return the data value stored at a certain index in the database.
     */
    static const UA_DataValue* _getDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        size_t              index);

    /**
     * Call-back returning UA_TRUE if the backend supports returning bounding values for a node.
     * It is mandatory.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node to test.
     * @return UA_TRUE if the backend supports returning bounding values for the node
     */
    static UA_Boolean _boundSupported(
        UA_Server*       server,
        void*            hdbContext,
        const UA_NodeId* sessionId,
        void*            sessionContext,
        const UA_NodeId* nodeId);

    /**
     * Call-back returning UA_TRUE if the backend supports returning the
     * requested timestamps for a given node.
     * It is mandatory.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node for which the capability to return
     *        certain timestamps shall be queried.
     * @return UA_TRUE if the backend supports returning the requested timestamps for the node
     */
    static UA_Boolean _timestampsToReturnSupported(
        UA_Server*                  server,
        void*                       hdbContext,
        const UA_NodeId*            sessionId,
        void*                       sessionContext,
        const UA_NodeId*            nodeId,
        const UA_TimestampsToReturn timestampsToReturn);

    /**
     * Call-back inserting a data value in a given node.
     * 
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node to modify.
     * @param value data value to insert.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static UA_StatusCode _insertDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    /**
     * Call-back replacing the data value of a given node. Time stamp modify?
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node to modify.
     * @param value new data value.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static  UA_StatusCode _replaceDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    /**
     * Call-back updating the data value of a given node. Time stamps not modify?
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node to modify.
     * @param value new data value.
     * @return UA_STATUSCODE_GOOD on success.
     */
    static UA_StatusCode _updateDataValue(
        UA_Server*          server,
        void*               hdbContext,
        const UA_NodeId*    sessionId,
        void*               sessionContext,
        const UA_NodeId*    nodeId,
        const UA_DataValue* value);

    /**
     * Call-back removing a node's data values in a given timestamp range.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId identify the session that wants to read historical data.
     * @param sessionContext the session context.
     * @param nodeId id of the node to modify.
     * @param startTimestamp first timestamp in the range, included.
     * @param endTimestamp last timestamp in the range, included.
     * @return UA_STATUSCODE_GOOD on success.
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
        memset(&m_database, 0, sizeof(m_database));
    }

    void setMemory(size_t nodes = 100, size_t size = 1000000) {
        m_database = UA_HistoryDataBackend_Memory(nodes, size);
    }

    /**
     * initialise to use class methods
     */
    void initialise();

    /**
     * Destructor
     */
    virtual ~HistoryDataBackend()       { deleteMembers(); }

    /** Return the underlying UA_HistoryDataBackend object. */
    UA_HistoryDataBackend& database()   { return m_database; }

    /**
     * Destroy managed members.
     * Hook customizing the _deleteMembers() call-back and the destructor.
     */
    virtual void deleteMembers()        {}

    /**
     * Sets a DataValue for a node in the historical data storage.
     * Hook customizing the _serverSetHistoryData() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param historizing is the historizing flag of the node identified by nodeId.
     *        If sessionId is NULL, the historizing flag is invalid and must not be used.
     * @param value is the value which shall be stored.
     * @return UA_STATUSCODE_GOOD on success.
     */
    virtual  UA_StatusCode serverSetHistoryData(
        Context&            context,
        bool                historizing,
        const UA_DataValue* value) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * Retrieve the History Data of a given node in the historical data storage.
     * Hook customizing _getHistoryData(), the high level HistoryRead API call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param start is the start time of the HistoryRead request.
     * @param end is the end time of the HistoryRead request.
     * @param nodeId is the id of the node for which historical data is requested.
     * @param maxSizePerResponse is the maximum number of items per response the server can provide.
     * @param numValuesPerNode is the maximum number of items per response the client wants to receive.
     * @param returnBounds determines if the client wants to receive bounding values.
     * @param timestampsToReturn specify which time stamps the client is interested in; device, server or both.
     * @param range is the numeric range the client wants to read.
     * @param releaseContinuationPoints determines if the continuation points shall be released.
     * @param continuationPoint is the continuation point the client wants to release or start from.
     * @param outContinuationPoint is the continuation point that gets passed to the
     *        client by the HistoryRead service.
     * @param result contains the result history data that gets passed to the client.
     * @return UA_STATUSCODE_GOOD on success.
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
     * Returns the index of a value in the database which matches certain criteria.
     * Hook customizing the _getDateTimeMatch() call-back part of the low level HistoryRead API.
     * Do nothing by default.
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
     * Returns the index of the element after the last valid entry 
     * in the database for a node.
     * Hook customizing the _getEnd() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param server is the server the node lives in.
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param sessionId and sessionContext identify the session that wants to read historical data.
     * @param nodeId is the id of the node for which the end of storage shall be returned.
     */
    virtual size_t getEnd(Context& hdbContext) {
        return 0;
    }
    
    /**
     * Returns the index of the last element in the database for a node.
     * Hook customizing the _lastIndex() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @return the index of the last element in the database for a node.
     */
    virtual size_t lastIndex(Context& hdbContext) {
        return 0;
    }

    /**
     * Returns the index of the first element in the database for a node.
     * Hook customizing the _firstIndex() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @return the index of the first element in the database for a node.
     */
    virtual size_t firstIndex(Context& hdbContext) {
        return 0;
    }
    
    /**
     * Returns the number of elements between startIndex and endIndex including both.
     * Hook customizing the _resultSize() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param hdbContext is the context of the UA_HistoryDataBackend.
     * @param startIndex is the index of the first element in the range.
     * @param endIndex is the index of the last element in the range.
     * @return the number of elements between startIndex and endIndex including both.
     */
    virtual size_t resultSize(Context& hdbContext, size_t startIndex, size_t endIndex) {
        return 0;
    }

    /**
     * Copies data values inside a certain range into a buffer.
     * Hook customizing the _copyDataValues() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
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
     * @return UA_STATUSCODE_GOOD on success.
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
        return UA_STATUSCODE_GOOD;
    }

    /**
     * Returns the data value stored at a certain index in the database.
     * Hook customizing the _getDataValue() call-back part of the low level HistoryRead API.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param index is the index in the database for which the data value is requested.
     * @return a pointer on the found DataValue.
     */
    virtual const UA_DataValue* getDataValue(Context& context, size_t index) {
        return nullptr;
    }

    /**
     * Returns UA_TRUE if the backend supports returning bounding
     * values for a node.
     * Hook customizing the mandatory _boundSupported() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @return UA_TRUE if the backend supports returning bounding, UA_FALSE otherwise.
     */
    virtual UA_Boolean boundSupported(Context& context) {
        return UA_FALSE;
    }

    /**
     * Returns UA_TRUE if the backend supports returning the
     * requested timestamps for a node.
     * Hook customizing the mandatory _timestampsToReturnSupported() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param sessionId and sessionContext identify the session that wants to read historical data.
     * @param nodeId is the id of the node for which the capability
     *        to return certain timestamps shall be queried.
     */
    virtual UA_Boolean timestampsToReturnSupported(Context& context, UA_TimestampsToReturn timestampsToReturn) {
        return UA_FALSE;
    }

    /**
     * Insert a data value in a given node.
     * Hook customizing the _insertDataValue() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param value data value to insert.
     * @return UA_STATUSCODE_GOOD on success.
     */
    virtual UA_StatusCode insertDataValue(Context& context, const UA_DataValue* value) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * Replace the data value of a given node. Time stamp modified?
     * Hook customizing the _replaceDataValue() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param value new data value.
     * @return UA_STATUSCODE_GOOD on success.
     */
    virtual UA_StatusCode replaceDataValue(Context& context, const UA_DataValue* value) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * Update the data value of a given node. Time stamps not modified?
     * Hook customizing the _updateDataValue() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param value new data value.
     * @return UA_STATUSCODE_GOOD on success.
     */
    virtual UA_StatusCode updateDataValue(Context& context, const UA_DataValue* value) {
        return UA_STATUSCODE_GOOD;
    }

    /**
     * Remove data values of node in a given timestamp range.
     * Hook customizing the _removeDataValue() call-back.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDataBackend.
     * @param startTimestamp first timestamp in the range, included.
     * @param endTimestamp last timestamp in the range, included.
     * @return UA_STATUSCODE_GOOD on success.
     */
    virtual UA_StatusCode removeDataValue(
        Context&    context,
        UA_DateTime startTimestamp,
        UA_DateTime endTimestamp) {
        return UA_STATUSCODE_GOOD;
    }
};

/**
 * The HistoryDatabase class
 * This is the historian storage database
 */
class HistoryDatabase {
    /**
    * Helper struct aggregating common call-backs arguments.
    */
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

    UA_HistoryDatabase m_database;

    static void _deleteMembers(UA_HistoryDatabase* hdb);

    /**
     * Call-back called when a nodes value is set.
     * Use this to insert data into your database(s) if polling is not suitable
     * and you need to get all data changes.
     * Set it to NULL if you do not need it.
     *
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId identify the session which set this value.
     * @param sessionContext the session context.
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
     * Call-back called if a history read is requested with isRawReadModified set to false.
     * Setting it to NULL will result in a response with status
     * code UA_STATUSCODE_BADHISTORYOPERATIONUNSUPPORTED.
     *
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId identify the session which set this value.
     * @param sessionContext the session context.
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
    
    /**
     * Call-back called when a nodes value is updated.
     *
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId identify the session which set this value.
     * @param sessionContext the details context.
     * @param requestHeader header for a server request
     * @param details specify the how the update must is done.
     *        specify the node affected.
     *        specify the type of operation (insert, replace, update, remove).
     *        specify an array of affected historical values.
     * @param result return an error code + an error and error details
     *        for each updated value split in 2 arrays.
     */
    static void _updateData(
        UA_Server*                  server,
        void*                       hdbContext,
        const UA_NodeId*            sessionId,
        void*                       sessionContext,
        const UA_RequestHeader*     requestHeader,
        const UA_UpdateDataDetails* details,
        UA_HistoryUpdateResult*     result);

    /**
     * Call-back called when a nodes isDeleteModified flag is modified.
     *
     * @param server is the server this node lives in.
     * @param hdbContext is the context of the UA_HistoryDatabase.
     * @param sessionId identify the session which set this value.
     * @param sessionContext the details context.
     * @param requestHeader header for a server request
     * @param details specify the how the update must be done.
     *        specify the node affected.
     *        specify the the value of the isDeleteModified flag.
     *        specify a timestamp range.
     * @param result return an error code + an error and error details
     *        for each updated value split in 2 arrays.
     */
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

    UA_HistoryDatabase& database()  { return m_database; }
    virtual void deleteMembers()    {}

    /**
    * initialise to use class methods
    */
    void initialise();

    /**
     * Hook called if a history read is requested with isRawReadModified set to false.
     * Setting it to NULL will result in a response with error code
     * UA_STATUSCODE_BADHISTORYOPERATIONUNSUPPORTED.
     * Do nothing by default.
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
        UA_HistoryData* const* const    historyData)
    {
    }
    
    /**
     * Hook called when a nodes value is updated.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDatabase.
     * @param requestHeader header for a server request
     * @param details specify the how the update must is done.
     *        specify the node affected.
     *        specify the type of operation (insert, replace, update, remove).
     *        specify an array of affected historical values.
     * @param result return an error code + an error and error details
     *        for each updated value split in 2 arrays.
     */
    virtual void updateData(
        Context&                    context,
        const UA_RequestHeader*     requestHeader,
        const UA_UpdateDataDetails* details,
        UA_HistoryUpdateResult*     result)
    {
    }
    
    /**
     * Hook called when a nodes isDeleteModified flag is modified.
     * Do nothing by default.
     *
     * @param context is the context of the UA_HistoryDatabase.
     * @param requestHeader header for a server request
     * @param details specify the how the update must be done.
     *        specify the node affected.
     *        specify the the value of the isDeleteModified flag.
     *        specify a timestamp range.
     * @param result return an error code + an error and error details
     *        for each updated value split in 2 arrays.
     */
    virtual void deleteRawModified(
        Context&                    context,
        const UA_RequestHeader*     requestHeader,
        const UA_DeleteRawModifiedDetails* details,
        UA_HistoryUpdateResult*     result)
    {
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
    UA_HistoryDatabase      m_database;
    UA_HistoryDataBackend   m_backend;
    UA_HistoryDataGathering m_gathering;

public:
    Historian();

    virtual ~Historian();

    // accessors
    UA_HistoryDatabase&         database()  { return m_database; }
    UA_HistoryDataGathering&    gathering() { return m_gathering; }
    UA_HistoryDataBackend&      backend()   { return m_backend; }

    /**
     * Registers a node for the gathering of historical data.
     * The values will be stored when a node is updated via write service.
     * @param nodeId id of the node to register.
     * @param server is the server the node lives in.
     * @param responseSize
     * @param pollInterval duration between 2 data polling in ms. 1s by default.
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
     * Registers a node for the gathering of historical data.
     * The value of the node will be read periodically.
     * Values will not be stored if the value is equal to the old value.
     * This is mainly relevant for data source nodes which do not use the write service.
     * @param nodeId id of the node to register.
     * @param server is the server the node lives in.
     * @param responseSize
     * @param pollInterval duration between 2 data polling in ms. 1s by default.
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
     * Registers a node for the gathering of historical data.
     * The user of the api stores the values to the database himself.
     * The api will not store any value to the database.
     * @param nodeId id of the node to register.
     * @param server is the server the node lives in.
     * @param responseSize
     * @param pollInterval duration between 2 data polling in ms. 1s by default.
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

#endif  // HISTORYDATABASE_H
