/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/monitoreditem.h>
#include <open62541cpp/open62541client.h>
#include <open62541cpp/clientsubscription.h>


namespace Open62541 {

    /*!
        \brief MonitoredItem::MonitoredItem
        \param s
    */
    MonitoredItem::MonitoredItem(ClientSubscription& s)
        : _sub(s)
    {
    }

/* Callback for the deletion of a MonitoredItem */
/* any of the parts may have disappeared */
/*!
    \brief MonitoredItem::deleteMonitoredItemCallback
    \param client
    \param subId
    \param subContext
    \param monId
    \param monContext
*/
void MonitoredItem::deleteMonitoredItemCallback(UA_Client* client,
                                                           UA_UInt32 subId,
                                                           void* /*subContext*/,
                                                           UA_UInt32 /*monId*/,
                                                           void* monContext)
{
    //
    // The subscription
    Client* cl = (Client*)UA_Client_getContext(client);
    if (cl && (cl->getConnectStatus() == UA_STATUSCODE_GOOD)) {
        ClientSubscription* c = cl->subscription(subId);
        if (c) {
            MonitoredItem* m = (MonitoredItem*)(monContext);
            if (m) {
                m->deleteMonitoredItem();
            }
        }
    }
}

/* Callback for DataChange notifications */
/*!
    \brief MonitoredItem::dataChangeNotificationCallback
    \param client
    \param subId
    \param subContext
    \param monId
    \param monContext
    \param value
*/
void MonitoredItem::dataChangeNotificationCallback(UA_Client* client,
                                                              UA_UInt32 subId,
                                                              void* /*subContext*/,
                                                              UA_UInt32 /*monId*/,
                                                              void* monContext,
                                                              UA_DataValue* value)
{
    Client* cl = (Client*)UA_Client_getContext(client);
    if (cl && (cl->getConnectStatus() == UA_STATUSCODE_GOOD)) {
        ClientSubscription* c = cl->subscription(subId);
        if (c) {
            MonitoredItem* m = (MonitoredItem*)(monContext);
            if (m) {
                m->dataChangeNotification(value);
            }
        }
    }
}

/* Callback for Event notifications */
/*!
    \brief MonitoredItem::eventNotificationCallback
    \param client
    \param subId
    \param subContext
    \param monId
    \param monContext
    \param nEventFields
    \param eventFields
*/
void MonitoredItem::eventNotificationCallback(UA_Client* client,
                                                         UA_UInt32 subId,
                                                         void* /*subContext*/,
                                                         UA_UInt32 /*monId*/,
                                                         void* monContext,
                                                         size_t nEventFields,
                                                         UA_Variant* eventFields)
{
    Client* cl = (Client*)UA_Client_getContext(client);
    if (cl && (cl->getConnectStatus() == UA_STATUSCODE_GOOD)) {
        ClientSubscription* c = cl->subscription(subId);
        if (c) {
            MonitoredItem* m = (MonitoredItem*)(monContext);
            if (m) {
                m->eventNotification(nEventFields, eventFields);
            }
        }
    }
}

//*****************************************************************************

bool  MonitoredItem::remove() {
    if (id() < 1 || !m_sub.client().client()) return false;
    
    bool ret = UA_Client_MonitoredItems_deleteSingle(
        m_sub.client().client(),
        m_sub.id(), id()) == UA_STATUSCODE_GOOD;
    m_response.null();
    
    return ret;
}


bool MonitoredItem::setMonitoringMode(
    const SetMonitoringModeRequest& request,
    SetMonitoringModeResponse&      response) {
    response = UA_Client_MonitoredItems_setMonitoringMode(
        subscription().client().client(),   // UA_Client*
        request.get());                     // UA_SetMonitoringModeResponse
    return true;
}

bool MonitoredItem::setTriggering(
    const SetTriggeringRequest& request,
    SetTriggeringResponse&      response) {
    response = UA_Client_MonitoredItems_setTriggering(
        subscription().client().client(),
        request.get());
    return true;
}


bool MonitoredItemDataChange::addDataChange(
    NodeId&                 node,
    UA_TimestampsToReturn   timeStamp /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    m_response = UA_Client_MonitoredItems_createDataChange(
        subscription().client().client(),
        subscription().id(),
        timeStamp, // source and/or server timestamp, or neither.
        UA_MonitoredItemCreateRequest_default(node),
        this,
        dataChangeNotificationCallback,
        deleteMonitoredItemCallback);
    return m_response->statusCode == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool MonitoredItemEvent::remove() {
    bool ret = MonitoredItem::remove();
    if (m_pEvents) delete m_pEvents;
    return ret;
}

//*****************************************************************************

void MonitoredItemEvent::eventNotification(
    size_t nEventFields, 
    UA_Variant* eventFields) {
    if (!m_func) return;
    
    VariantArray va(eventFields, nEventFields);
    m_func(subscription(), va); // invoke functor
    va.release();
}

//*****************************************************************************

bool MonitoredItemEvent::addEvent(
    NodeId&                 node,
    EventFilterSelect*      events,
    UA_TimestampsToReturn   timeStamp /*= UA_TIMESTAMPSTORETURN_BOTH*/) {
    if (!events) return false;
    
    remove(); // delete any existing events

    m_pEvents = events; // take ownership - events must be deleted after the item is removed
    MonitoredItemCreateRequest item;
    item = UA_MonitoredItemCreateRequest_default(node);
    item->itemToMonitor.nodeId = node;
    item->itemToMonitor.attributeId            = UA_ATTRIBUTEID_EVENTNOTIFIER;
    item->monitoringMode                       = UA_MONITORINGMODE_REPORTING;
    item->requestedParameters.filter.encoding  = UA_EXTENSIONOBJECT_DECODED;
    item->requestedParameters.filter.content.decoded.data = events->ref();
    item->requestedParameters.filter.content.decoded.type = &UA_TYPES[UA_TYPES_EVENTFILTER];

    m_response = UA_Client_MonitoredItems_createEvent(
        subscription().client().client(),
        subscription().id(),
        timeStamp, // source and/or server timestamp, or neither.
        item,
        this,
        eventNotificationCallback,
        deleteMonitoredItemCallback);
    return m_response->statusCode == UA_STATUSCODE_GOOD;
}

} // namespace Open62541
