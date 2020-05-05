/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <clientsubscription.h>
#include <open62541client.h>

namespace Open62541 {

ClientSubscription::ClientSubscription(Client& client)
    : _client(client) {
    _settings = UA_CreateSubscriptionRequest_default();
}

//*****************************************************************************

ClientSubscription::~ClientSubscription() {
    if (!id()) return;
    
    _map.clear(); // delete all monitored items
    if (_client.client())
        UA_Client_Subscriptions_deleteSingle(_client.client(), id());
}

//*****************************************************************************

bool ClientSubscription::create() {
    if (!_client.client()) return false;
    
    _response = UA_Client_Subscriptions_create(
        _client.client(),
        _settings,
        (void*)(this),
        statusChangeNotificationCallback,
        deleteSubscriptionCallback);
    _lastError = _response->responseHeader.serviceResult;
    return _lastError == UA_STATUSCODE_GOOD;
}

//*****************************************************************************

unsigned ClientSubscription::addMonitorItem(const MonitoredItemRef& item) {
    _map[++_monitorId] = item;
    return _monitorId;
}

//*****************************************************************************

void ClientSubscription::deleteMonitorItem(unsigned id) {
    if (_map.find(id) != _map.end()) {
        _map[id]->remove();
        _map.erase(id);
    }
}

//*****************************************************************************

MonitoredItem* ClientSubscription::findMonitorItem(unsigned id) {
    if (_map.find(id) != _map.end()) {
        return _map[id].get();
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
