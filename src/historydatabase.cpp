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

} // namespace Open62541
