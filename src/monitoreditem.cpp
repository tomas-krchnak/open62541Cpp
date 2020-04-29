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

MonitoredItem::MonitoredItem(ClientSubscription& s) : _sub(s) {

}

void MonitoredItem::deleteMonitoredItemCallback
(UA_Client* /*client*/, UA_UInt32 /*subId*/, void* subContext,
 UA_UInt32 /*monId*/, void* monContext) {
    MonitoredItem* m = (MonitoredItem*)(monContext);
    ClientSubscription* c = (ClientSubscription*)subContext;
    if (m && c) {
        m->deleteMonitoredItem();
    }
}

void MonitoredItem::dataChangeNotificationCallback
(UA_Client* /*client*/, UA_UInt32 /*subId*/, void* subContext,
 UA_UInt32 /*monId*/, void* monContext,
 UA_DataValue* value) {
    MonitoredItem* m = (MonitoredItem*)(monContext);
    ClientSubscription* c = (ClientSubscription*)subContext;
    if (m && c) {
        m->dataChangeNotification(value);
    }
}

void MonitoredItem::eventNotificationCallback
(UA_Client* /*client*/, UA_UInt32 /*subId*/, void* subContext,
 UA_UInt32 /*monId*/, void* monContext,
 size_t nEventFields, UA_Variant* eventFields) {
    MonitoredItem* m = (MonitoredItem*)(monContext);
    ClientSubscription* c = (ClientSubscription*)subContext;
    if (m && c) {
        m->eventNotification(nEventFields, eventFields);
    }
}

bool  MonitoredItem::remove() {
    bool ret =  false;
    if ((id() > 0) && _sub.client().client() ) {
        ret = UA_Client_MonitoredItems_deleteSingle(_sub.client().client(), _sub.id(), id()) == UA_STATUSCODE_GOOD;
        _response.null();
    }
    return ret;
}

bool  MonitoredItem::setMonitoringMode( const SetMonitoringModeRequest& request, SetMonitoringModeResponse& response)
{
    response.get() = UA_Client_MonitoredItems_setMonitoringMode(subscription().client().client(), request.get());
    return true;
}

bool  MonitoredItem::setTriggering(const SetTriggeringRequest& request, SetTriggeringResponse& response)
{
    response.get() =  UA_Client_MonitoredItems_setTriggering(subscription().client().client(), request.get());
    return true;
}

bool MonitoredItemDataChange::addDataChange(NodeId& n, UA_TimestampsToReturn ts) {
    MonitoredItemCreateRequest monRequest;
    monRequest = UA_MonitoredItemCreateRequest_default(n);
    _response.get() = UA_Client_MonitoredItems_createDataChange(subscription().client().client(),
                                                                subscription().id(),
                                                                ts,
                                                                monRequest,
                                                                this,
                                                                dataChangeNotificationCallback,
                                                                deleteMonitoredItemCallback);
    return _response.get().statusCode == UA_STATUSCODE_GOOD;
}

bool MonitoredItemEvent::addEvent(NodeId& n, EventFilterSelect* events, UA_TimestampsToReturn ts) {
    if (events) {
        remove(); // delete any existing item

        _events = events; // take ownership - events must be deleted after the item is removed
        MonitoredItemCreateRequest item;
        item = UA_MonitoredItemCreateRequest_default(n);

        item.get().itemToMonitor.nodeId = n;
        item.get().itemToMonitor.attributeId = UA_ATTRIBUTEID_EVENTNOTIFIER;
        item.get().monitoringMode = UA_MONITORINGMODE_REPORTING;

        item.get().requestedParameters.filter.encoding = UA_EXTENSIONOBJECT_DECODED;
        item.get().requestedParameters.filter.content.decoded.data = events->ref();
        item.get().requestedParameters.filter.content.decoded.type = &UA_TYPES[UA_TYPES_EVENTFILTER];

        _response = UA_Client_MonitoredItems_createEvent(subscription().client().client(),
                                                         subscription().id(),
                                                         ts,
                                                         item,
                                                         this,
                                                         eventNotificationCallback,
                                                         deleteMonitoredItemCallback);
        return _response.get().statusCode == UA_STATUSCODE_GOOD;
    }
    return false;
}

} // namespace Open62541
