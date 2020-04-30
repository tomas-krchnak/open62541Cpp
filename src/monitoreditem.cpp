/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <monitoreditem.h>
#include <open62541client.h>
#include <clientsubscription.h>

namespace Open62541 {

void MonitoredItem::deleteMonitoredItemCallback(
    UA_Client* /*client*/,
    UA_UInt32  /*subId*/,
    void*        subContext,
    UA_UInt32  /*monId*/,
    void*        monContext) {
    auto m = (MonitoredItem*)monContext;
    auto c = (ClientSubscription*)subContext;

    if (m && c) {
        m->deleteMonitoredItem();
    }
}

//*****************************************************************************

void MonitoredItem::dataChangeNotificationCallback(
    UA_Client*    /*client*/,
    UA_UInt32     /*subId*/,
    void*           subContext,
     UA_UInt32    /*monId*/,
    void*           monContext,
     UA_DataValue*  outNewData) {
    auto m = (MonitoredItem*)monContext;
    auto c = (ClientSubscription*)subContext;

    if (m && c) {
        m->dataChangeNotification(outNewData);
    }
}

//*****************************************************************************

void MonitoredItem::eventNotificationCallback(
    UA_Client*    /*client*/,
    UA_UInt32     /*subId*/,
    void*           subContext,
    UA_UInt32     /*monId*/,
    void*           monContext,
    size_t          nEventFields,
    UA_Variant*     eventFields) {
    auto m = (MonitoredItem*)monContext;
    auto c = (ClientSubscription*)subContext;
    if (m && c) {
        m->eventNotification(nEventFields, eventFields);
    }
}

//*****************************************************************************

bool  MonitoredItem::remove() {
    if (id() < 1 || !_sub.client().client()) return false;
    
    bool ret = UA_Client_MonitoredItems_deleteSingle(
        _sub.client().client(),
        _sub.id(), id()) == UA_STATUSCODE_GOOD;
    _response.null();
    
    return ret;
}

//*****************************************************************************

bool MonitoredItem::setMonitoringMode(
    const SetMonitoringModeRequest& request,
    SetMonitoringModeResponse&      response) {
    response.get() = UA_Client_MonitoredItems_setMonitoringMode(
        subscription().client().client(),   // UA_Client*
        request.get());                     // UA_SetMonitoringModeResponse*
    return true;
}

//*****************************************************************************

bool MonitoredItem::setTriggering(
    const SetTriggeringRequest& request,
    SetTriggeringResponse&      response) {
    response.get() = UA_Client_MonitoredItems_setTriggering(
        subscription().client().client(),
        request.get());
    return true;
}

//*****************************************************************************

bool MonitoredItemDataChange::addDataChange(
    NodeId&                 node,
    UA_TimestampsToReturn   timeStamp /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    _response.get() = UA_Client_MonitoredItems_createDataChange(
        subscription().client().client(),
        subscription().id(),
        timeStamp, // source and/or server timestamp, or neither.
        UA_MonitoredItemCreateRequest_default(node),
        this,
        dataChangeNotificationCallback,
        deleteMonitoredItemCallback);
    return _response.get().statusCode == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool MonitoredItemEvent::remove() {
    bool ret = MonitoredItem::remove();
    if (_events) delete _events;
    return ret;
}

//*****************************************************************************

void MonitoredItemEvent::eventNotification(size_t nEventFields, UA_Variant* eventFields) {
    if (!_func) return;
    
    VariantArray va;
    va.setList(nEventFields, eventFields);
    _func(subscription(), va); // invoke functor
    va.release();
}

//*****************************************************************************

bool MonitoredItemEvent::addEvent(
    NodeId&                 node,
    EventFilterSelect*      events,
    UA_TimestampsToReturn   timeStamp /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    if (!events) return false;
    
    remove(); // delete any existing events

    _events = events; // take ownership - events must be deleted after the item is removed
    MonitoredItemCreateRequest item;
    item = UA_MonitoredItemCreateRequest_default(node);
    item.get().itemToMonitor.nodeId = node;
    item.get().itemToMonitor.attributeId            = UA_ATTRIBUTEID_EVENTNOTIFIER;
    item.get().monitoringMode                       = UA_MONITORINGMODE_REPORTING;
    item.get().requestedParameters.filter.encoding  = UA_EXTENSIONOBJECT_DECODED;
    item.get().requestedParameters.filter.content.decoded.data = events->ref();
    item.get().requestedParameters.filter.content.decoded.type = &UA_TYPES[UA_TYPES_EVENTFILTER];

    _response = UA_Client_MonitoredItems_createEvent(
        subscription().client().client(),
        subscription().id(),
        timeStamp, // source and/or server timestamp, or neither.
        item,
        this,
        eventNotificationCallback,
        deleteMonitoredItemCallback);
    return _response.get().statusCode == UA_STATUSCODE_GOOD;
}

} // namespace Open62541
