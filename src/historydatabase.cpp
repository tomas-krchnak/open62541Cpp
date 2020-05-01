/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "historydatabase.h"

namespace Open62541 {

void HistoryDataGathering::_deleteMembers(UA_HistoryDataGathering* gathering) {
    if (gathering && gathering->context) {
        auto p = static_cast<HistoryDataGathering*>(gathering->context);
        p->deleteMembers();
    }
}

//*****************************************************************************

UA_StatusCode HistoryDataGathering::_registerNodeId(
    UA_Server*                          server,
    void*                               hdgContext,
    const UA_NodeId*                    nodeId,
    const UA_HistorizingNodeIdSettings  setting) {
    if (!hdgContext) return 0;
    
    Context c(server, nodeId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->registerNodeId(c, setting);
}

//*****************************************************************************

UA_StatusCode HistoryDataGathering::_stopPoll(
    UA_Server*       server,
    void*            hdgContext,
    const UA_NodeId* nodeId) {
    if (!hdgContext) return 0;

    Context c(server, nodeId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->stopPoll(c);
}

//*****************************************************************************

UA_StatusCode HistoryDataGathering::_startPoll(
    UA_Server*       server,
    void*            hdgContext,
    const UA_NodeId* nodeId) {
    if (!hdgContext) return 0;

    Context c(server, nodeId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->startPoll(c);
}

//*****************************************************************************

UA_Boolean HistoryDataGathering::_updateNodeIdSetting(
    UA_Server*                          server,
    void*                               hdgContext,
    const UA_NodeId*                    nodeId,
    const UA_HistorizingNodeIdSettings  setting) {
    if (!hdgContext) return 0;

    Context c(server, nodeId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->updateNodeIdSetting(c, setting);
}

//*****************************************************************************

const UA_HistorizingNodeIdSettings* HistoryDataGathering::_getHistorizingSetting(
    UA_Server*          server,
    void*               hdgContext,
    const UA_NodeId*    nodeId) {
    if (!hdgContext) return 0;

    Context c(server, nodeId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->getHistorizingSetting(c);
}

//*****************************************************************************

void HistoryDataGathering::_setValue(
    UA_Server*          server,
    void*               hdgContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    UA_Boolean          historizing,
    const UA_DataValue* value) {
    if (!hdgContext) return;

    Context context(server, nodeId);
    context.sessionContext = sessionContext;
    if (sessionId) context.sessionId.assignFrom(*sessionId);
    auto p = static_cast<HistoryDataGathering*>(hdgContext);
    return p->setValue(context, historizing, value);
}

//*****************************************************************************

void HistoryDataGathering::initialise() {
    _gathering.registerNodeId        = _registerNodeId;
    _gathering.deleteMembers         = _deleteMembers;
    _gathering.getHistorizingSetting = _getHistorizingSetting;
    _gathering.setValue              = _setValue;
    _gathering.startPoll             = _startPoll;
    _gathering.stopPoll              = _stopPoll;
    _gathering.updateNodeIdSetting   = _updateNodeIdSetting;
    _gathering.context               = this;
}

//*****************************************************************************

void HistoryDataBackend::_deleteMembers(UA_HistoryDataBackend* backend) {
    if (!backend || !backend->context) return;

    auto p = static_cast<HistoryDataBackend*>(backend->context);
    p->deleteMembers(); // destructor close handles etc
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_serverSetHistoryData(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    UA_Boolean          historizing,
    const UA_DataValue* value) {
    if (!hdbContext || !sessionId) return UA_STATUSCODE_GOOD; // ignore

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->serverSetHistoryData(c, historizing, value);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_getHistoryData(
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
    UA_HistoryData*         result) {
    if (!backend || !backend->context) return UA_STATUSCODE_GOOD; // ignore
    
    Context context(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(backend->context);
    std::string in = fromByteString(*(const_cast<UA_ByteString*>(continuationPoint)));
    std::string out;

    UA_StatusCode ret = p->getHistoryData(
        context,
        start,
        end,
        maxSizePerResponse,
        numValuesPerNode,
        returnBounds,
        timestampsToReturn,
        range,
        releaseContinuationPoints,
        in,
        out,
        result);
    *outContinuationPoint = UA_BYTESTRING(const_cast<char*>(out.c_str()));
    return ret;
}

//*****************************************************************************

size_t HistoryDataBackend::_getDateTimeMatch(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    const UA_DateTime   timestamp,
    const MatchStrategy strategy) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->getDateTimeMatch(c, timestamp, strategy);
}

//*****************************************************************************

size_t HistoryDataBackend::_getEnd(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->getEnd(c);
}

//*****************************************************************************

size_t HistoryDataBackend::_lastIndex(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->lastIndex(c);
}

//*****************************************************************************

size_t HistoryDataBackend::_firstIndex(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->firstIndex(c);
}

//*****************************************************************************

size_t HistoryDataBackend::_resultSize(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    size_t              startIndex,
    size_t              endIndex) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->resultSize(c, startIndex, endIndex);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_copyDataValues(
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
    UA_DataValue*       values) {
    if (!hdbContext || !sessionId) return 0;

    Context context(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    std::string in = fromByteString(*(const_cast<UA_ByteString*>(continuationPoint)));
    std::string out;
    UA_StatusCode ret = p->copyDataValues(
        context,
        startIndex,
        endIndex,
        reverse,
        valueSize,
        range,
        releaseContinuationPoints,
        in,
        out,
        providedValues,
        values);
    *outContinuationPoint = UA_BYTESTRING(const_cast<char*>(out.c_str()));
    return ret;
}

//*****************************************************************************

const UA_DataValue* HistoryDataBackend::_getDataValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    size_t              index) {
    if (!hdbContext || !sessionId) return nullptr;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->getDataValue(c, index);
}

//*****************************************************************************

UA_Boolean HistoryDataBackend::_boundSupported(
    UA_Server*       server,
    void*            hdbContext,
    const UA_NodeId* sessionId,
    void*            sessionContext,
    const UA_NodeId* nodeId) {
    if (!hdbContext || !sessionId) return UA_FALSE;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->boundSupported(c);
}

//*****************************************************************************

UA_Boolean HistoryDataBackend::_timestampsToReturnSupported(
    UA_Server*                  server,
    void*                       hdbContext,
    const UA_NodeId*            sessionId,
    void*                       sessionContext,
    const UA_NodeId*            nodeId,
    const UA_TimestampsToReturn timestampsToReturn) {
    if (!hdbContext || !sessionId) return UA_FALSE;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->timestampsToReturnSupported(c, timestampsToReturn);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_insertDataValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    const UA_DataValue* value) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->insertDataValue(c, value);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_replaceDataValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    const UA_DataValue* value) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->replaceDataValue(c, value);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_updateDataValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    const UA_DataValue* value) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->updateDataValue(c, value);
}

//*****************************************************************************

UA_StatusCode HistoryDataBackend::_removeDataValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    UA_DateTime         startTimestamp,
    UA_DateTime         endTimestamp) {
    if (!hdbContext || !sessionId) return 0;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDataBackend*>(hdbContext);
    return p->removeDataValue(c, startTimestamp, endTimestamp);
}

//*****************************************************************************

void HistoryDataBackend::initialise() {
    memset(&_database, 0, sizeof(_database));
    _database.context                       = this;
    // set up the static callback methods
    _database.boundSupported                = _boundSupported;
    _database.copyDataValues                = _copyDataValues;
    _database.deleteMembers                 = _deleteMembers;
    _database.firstIndex                    = _firstIndex;
    _database.getDataValue                  = _getDataValue;
    _database.getDateTimeMatch              = _getDateTimeMatch;
    _database.getEnd                        = _getEnd;
    _database.getHistoryData                = _getHistoryData;
    _database.insertDataValue               = _insertDataValue;
    _database.lastIndex                     = _lastIndex;
    _database.removeDataValue               = _removeDataValue;
    _database.replaceDataValue              = _replaceDataValue;
    _database.resultSize                    = _resultSize;
    _database.serverSetHistoryData          = _serverSetHistoryData;
    _database.timestampsToReturnSupported   = _timestampsToReturnSupported;
    _database.updateDataValue               = _updateDataValue;
}

//*****************************************************************************

void HistoryDatabase::_deleteMembers(UA_HistoryDatabase* hdb) {
    if (!hdb || !hdb->context) return;
    
    auto p = static_cast<HistoryDatabase*>(hdb->context);
    p->deleteMembers();
}

//*****************************************************************************

void HistoryDatabase::_setValue(
    UA_Server*          server,
    void*               hdbContext,
    const UA_NodeId*    sessionId,
    void*               sessionContext,
    const UA_NodeId*    nodeId,
    UA_Boolean          historizing,
    const UA_DataValue* value) {
    if (!hdbContext) return;

    Context c(server, sessionId, sessionContext, nodeId);
    auto p = static_cast<HistoryDatabase*>(hdbContext);
    p->setValue(c, historizing, value);
}

//*****************************************************************************

void HistoryDatabase::_readRaw(
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
    UA_HistoryData* const* const    historyData) {
    if (!hdbContext) return;

    Context context(server, sessionId, sessionContext, sessionId);
    auto p = static_cast<HistoryDatabase*>(hdbContext);
    p->readRaw(
        context,
        requestHeader,
        historyReadDetails,
        timestampsToReturn,
        releaseContinuationPoints,
        nodesToReadSize,
        nodesToRead,
        response,
        historyData);
}

//*****************************************************************************

void HistoryDatabase::_updateData(
    UA_Server*                  server,
    void*                       hdbContext,
    const UA_NodeId*            sessionId,
    void*                       sessionContext,
    const UA_RequestHeader*     requestHeader,
    const UA_UpdateDataDetails* details,
    UA_HistoryUpdateResult*     result) {
    if (!hdbContext) return;

    Context c(server, sessionId, sessionContext, sessionId);
    auto p = static_cast<HistoryDatabase*>(hdbContext);
    p->updateData(c, requestHeader, details, result);
}

//*****************************************************************************

void HistoryDatabase::_deleteRawModified(
    UA_Server*                          server,
    void*                               hdbContext,
    const UA_NodeId*                    sessionId,
    void*                               sessionContext,
    const UA_RequestHeader*             requestHeader,
    const UA_DeleteRawModifiedDetails*  details,
    UA_HistoryUpdateResult*             result) {
    if (!hdbContext) return;

    Context c(server, sessionId, sessionContext, sessionId);
    auto p = static_cast<HistoryDatabase*>(hdbContext);
    p->deleteRawModified(c, requestHeader, details, result);
}

//*****************************************************************************

Historian::Historian() {
    memset(&_database, 0, sizeof(_database));
    memset(&_backend, 0, sizeof(_backend));
    memset(&_gathering, 0, sizeof(_gathering));
}

//*****************************************************************************

Historian::~Historian() {
    if (_backend.context)
        UA_HistoryDataBackend_Memory_deleteMembers(&_backend);
}

//*****************************************************************************

bool Historian::setUpdateNode(
    NodeId& nodeId,
    Server& server,
    size_t  responseSize,
    size_t  pollInterval,
    void*   context)
{
    UA_HistorizingNodeIdSettings setting;
    setting.pollingInterval             = pollInterval;
    setting.historizingBackend          = _backend; // set the memory database
    setting.maxHistoryDataResponseSize  = responseSize;
    setting.historizingUpdateStrategy   = UA_HISTORIZINGUPDATESTRATEGY_VALUESET;
    setting.userContext                 = context;
    return gathering().registerNodeId(
        server.server(),
        gathering().context,
        nodeId.ref(),
        setting) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Historian::setPollNode(
    NodeId& nodeId,
    Server& server,
    size_t  responseSize,
    size_t  pollInterval,
    void*   context)
{
    UA_HistorizingNodeIdSettings setting;
    setting.historizingBackend          = _backend; // set the memory database
    setting.pollingInterval             = pollInterval;
    setting.maxHistoryDataResponseSize  = responseSize;
    setting.historizingUpdateStrategy   = UA_HISTORIZINGUPDATESTRATEGY_POLL;
    setting.userContext                 = context;
    return gathering().registerNodeId(
        server.server(),
        gathering().context,
        nodeId.ref(),
        setting) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool Historian::setUserNode(
    NodeId& nodeId,
    Server& server,
    size_t  responseSize,
    size_t  pollInterval,
    void*   context)
{
    UA_HistorizingNodeIdSettings setting;
    setting.historizingBackend          = _backend; // set the memory database
    setting.pollingInterval             = pollInterval;
    setting.maxHistoryDataResponseSize  = responseSize;
    setting.historizingUpdateStrategy   = UA_HISTORIZINGUPDATESTRATEGY_USER;
    setting.userContext                 = context;
    return gathering().registerNodeId(
        server.server(),
        gathering().context,
        nodeId.ref(),
        setting) == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

MemoryHistorian::MemoryHistorian(
    size_t numberNodes      /*= 100*/,
    size_t maxValuesPerNode /*= 100*/) {
    gathering() = UA_HistoryDataGathering_Default(numberNodes);
    database()  = UA_HistoryDatabase_default(gathering());
    backend()   = UA_HistoryDataBackend_Memory(numberNodes, maxValuesPerNode);
}

} // namespace Open62541
