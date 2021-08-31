/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/clientsubscription.h>
#include <open62541cpp/open62541client.h>

namespace Open62541 {
void  Open62541::ClientSubscription::deleteSubscriptionCallback(
    UA_Client *client, 
    UA_UInt32 subId, 
    void *subscriptionContext) {
    Client * cl = (Client *)UA_Client_getContext(client);
    if(cl)
    {
        ClientSubscription * c = cl->subscription(subId);
        if (c)c->deleteSubscription();
    }
}

/*!
    \brief statusChangeNotificationCallback
    \param subscriptionContext
    \param notification
*/

void ClientSubscription::statusChangeNotificationCallback(UA_Client* client,
                                                          UA_UInt32 subId,
                                                          void* /*subscriptionContext*/,
                                                          UA_StatusChangeNotification* notification)
{
    Client* cl = (Client*)UA_Client_getContext(client);
    if (cl) {
        ClientSubscription* c = cl->subscription(subId);
        if (c)
            c->statusChangeNotification(notification);
    }
}


ClientSubscription::ClientSubscription(Client& client)
    : m_client(client) {
    m_settings = UA_CreateSubscriptionRequest_default();
}

//*****************************************************************************

ClientSubscription::~ClientSubscription() {
    if (!id()) return;
    
    m_map.clear(); // delete all monitored items
    if (m_client.client())
        UA_Client_Subscriptions_deleteSingle(m_client.client(), id());
}

//*****************************************************************************

bool ClientSubscription::create() {
    if (!m_client.client()) return false;
    
    m_response = UA_Client_Subscriptions_create(
        m_client.client(),
        m_settings,
        (void*)(this),
        statusChangeNotificationCallback,
        deleteSubscriptionCallback);
    return (m_response->responseHeader.serviceResult == UA_STATUSCODE_GOOD);
}

//*****************************************************************************

unsigned ClientSubscription::addMonitorItem(const MonitoredItemRef& item) {
    m_map[++m_monitorId] = item;
    return m_monitorId;
}

//*****************************************************************************

void ClientSubscription::deleteMonitorItem(unsigned id) {
    if (m_map.find(id) != m_map.end()) {
        m_map[id]->remove();
        m_map.erase(id);
    }
}

//*****************************************************************************

MonitoredItem* ClientSubscription::findMonitorItem(unsigned id) {
    if (m_map.find(id) != m_map.end()) {
        return m_map[id].get();
    }
    return nullptr;
}

//*****************************************************************************

unsigned ClientSubscription::addMonitorNodeId(monitorItemFunc func, NodeId& node) {
    auto pdc = new MonitoredItemDataChange(func, *this);

    if (pdc->addDataChange(node)) {                   // make it notify on data change
        return addMonitorItem(MonitoredItemRef(pdc)); // add to subscription set
    }

    delete pdc;
    return 0; // item id
}

//*****************************************************************************

unsigned ClientSubscription::addEventMonitor(
    monitorEventFunc    func,
    NodeId&             node,
    EventFilterSelect*  filter) {
    auto pdc = new MonitoredItemEvent(func, *this);

    if (pdc->addEvent(node, filter)) {                // make it notify on data change
        return addMonitorItem(MonitoredItemRef(pdc)); // add to subscription set
    }

    delete pdc;
    return 0; // item id
}

} // namespace Open62541
