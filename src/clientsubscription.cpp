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

ClientSubscription::ClientSubscription(Client& client) : _client(client) {
    _settings.get() = UA_CreateSubscriptionRequest_default();
}

//*****************************************************************************

ClientSubscription::~ClientSubscription() {
    if (id()) {
        _map.clear(); // delete all monitored items
        if (_client.client())
            UA_Client_Subscriptions_deleteSingle(_client.client(), id());
    }
}

//*****************************************************************************

bool ClientSubscription::create() {
    if (_client.client()) {
        _response.get() = UA_Client_Subscriptions_create(
            _client.client(),
            _settings,
            (void*)(this),
            statusChangeNotificationCallback,
            deleteSubscriptionCallback);
        _lastError = _response.get().responseHeader.serviceResult;
        return _lastError == UA_STATUSCODE_GOOD;
    }
    return false;
}

//*****************************************************************************

unsigned ClientSubscription::addMonitorNodeId(monitorItemFunc func, NodeId& node) {
    unsigned ret = 0;
    auto pdc = new MonitoredItemDataChange(func, *this);
    if (pdc->addDataChange(node)) { // make it notify on data change
        MonitoredItemRef mcd(pdc);
        ret = addMonitorItem(mcd); // add to subscription set
    }
    else {
        delete pdc;
    }
    return ret; // returns item id
}

//*****************************************************************************

unsigned ClientSubscription::addEventMonitor(
    monitorEventFunc    func,
    NodeId&             node,
    EventFilterSelect*  filter) {
    unsigned ret = 0; // item id
    auto pdc = new MonitoredItemEvent(func, *this);
    if (pdc->addEvent(node, filter)) { // make it notify on data change
        MonitoredItemRef mcd(pdc);
        ret = addMonitorItem(mcd); // add to subscription set
    }
    else {
        delete pdc;
    }
    return ret;
}

} // namespace Open62541
