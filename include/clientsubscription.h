/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#ifndef CLIENTSUBSCRIPTION_H
#define CLIENTSUBSCRIPTION_H
#include "open62541objects.h"
#include "monitoreditem.h"

namespace Open62541 {

/**
 * The ClientSubscription class
 * Encapsulates a client subscription
 */

typedef std::shared_ptr<MonitoredItem> MonitoredItemRef;

typedef std::map<unsigned, MonitoredItemRef> MonitoredItemMap;

class UA_EXPORT ClientSubscription {
    Client &_client;  // owning client
    CreateSubscriptionRequest _settings;
    CreateSubscriptionResponse _response;

    int _monitorId = 0; // key monitor items by Id
    MonitoredItemMap _map; // map of monitor items - these are monitored items owned by this subscription

protected:
    UA_StatusCode _lastError = 0;
    /**
     * deleteSubscriptionCallback
     * @param subscriptionContext
     */
    static void  deleteSubscriptionCallback(UA_Client *, UA_UInt32, void *subscriptionContext) {
        ClientSubscription *p = (ClientSubscription *)(subscriptionContext);
        if (p)p->deleteSubscription();
    }

    /**
     * statusChangeNotificationCallback
     * @param subscriptionContext
     * @param notification
     */

    static void statusChangeNotificationCallback(UA_Client * /*client*/, UA_UInt32 /*subId*/, void *subscriptionContext,
                                                    UA_StatusChangeNotification *notification) {
        ClientSubscription *p = (ClientSubscription *)(subscriptionContext);
        if (p)p->statusChangeNotification(notification);
    }

public:
    /**
     * ClientSubscription
     * @param c
     */
    ClientSubscription(Client &c);

    /**
     * ~ClientSubscription
     * Only delete subscriptions from the client
     */
    virtual ~ClientSubscription();

    /**
     * create
     * @return true on success
     */
    bool create();


    /**
     * client
     * @return reference to owning client
     */
    Client &client() {
        return _client;
    }

    /**
     * id
     * @return subscription id
     */
    UA_UInt32 id() {
        return _response.get().subscriptionId;
    }

    /**
     * deleteSubscriptionCallback
     */
    virtual void  deleteSubscription() {}

    /**
     * changeNotificationCallback
     */
    virtual void  statusChangeNotification(UA_StatusChangeNotification * /*notification*/) {}

    /**
     * settings
     * @return reference to the request structure
     */
    UA_CreateSubscriptionRequest &settings() {
        return  _settings;
    }

    /**
     * response
     * @return reference to subscription response
     */
    UA_CreateSubscriptionResponse &response() {
        return _response;
    }

    /**
     * addMonitorItem
     * @param m monitored item
     * @return monitored item
     */
    unsigned addMonitorItem(MonitoredItemRef &m) {
        _monitorId++;
        _map[_monitorId] = m;
        return _monitorId;
    }

    /**
     * deleteMonitorItem
     * @param id Id of the monitored item (from addMonitorItem) to delete
     */

    void deleteMonitorItem(unsigned id) {
        if (_map.find(id) != _map.end()) {
            MonitoredItemRef &m = _map[id];
            m->remove();
            _map.erase(id);
        }
    }

    /**
     * findMonitorItem
     * @param id Id of monitored item
     * @return pointer to MonitoredItem or null
     */
    MonitoredItem *findMonitorItem(unsigned id) {
        if (_map.find(id) != _map.end()) {
            MonitoredItemRef &m = _map[id];
            return m.get();
        }
        return nullptr;
    }

    /**
     * addMonitorNodeId
     * @param f Functor to handle item updates
     * @param n node to monitor
     */
    unsigned addMonitorNodeId(monitorItemFunc f, NodeId &n);

    /**
     * addEventMonitor
     * @param f functor to handle event
     * @param n node to monitor
     * @param ef event filter
     */
    unsigned addEventMonitor(monitorEventFunc f, NodeId &n, Open62541::EventFilterSelect *ef);
};

} // namespace Open62541

#endif // CLIENTSUBSCRIPTION_H
