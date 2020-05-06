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

typedef std::shared_ptr<MonitoredItem> MonitoredItemRef;
typedef std::map<unsigned, MonitoredItemRef> MonitoredItemMap;

/**
 * The ClientSubscription class
 * Encapsulates a client subscription.
 * Note the difference between Subscriptions and MonitoredItems.
 * Subscriptions are used to report back notifications.
 * MonitoredItems are used to generate notifications.
 * Every MonitoredItem is attached to exactly one Subscription.
 * And a Subscription can contain many MonitoredItems.
 */
class UA_EXPORT ClientSubscription {
    Client&                     _client;        /**< owning client */
    CreateSubscriptionRequest   _settings;      /**< subscription settings */
    CreateSubscriptionResponse  _response;      /**< subscription response */
    int                         _monitorId = 0; /**< key monitor items by Id */
    MonitoredItemMap            _map;           /**< map of monitor items - these are monitored items owned by this subscription */

protected:
    /**
     * Call-back called when the subscription ends
     * @param subscriptionContext
     */
    static void  deleteSubscriptionCallback(
        UA_Client*  client,
        UA_UInt32   subId,
        void*       subscriptionContext) {
        if (auto p = (ClientSubscription*)(subscriptionContext))
            p->deleteSubscription();
    }

    /**
     * Call-back called when the monitored item changes
     * @param subscriptionContext
     * @param notification
     */
    static void statusChangeNotificationCallback(
        UA_Client*                      client,
        UA_UInt32                       subId,
        void*                           subscriptionContext,
        UA_StatusChangeNotification*    notification) {
        if (auto p = (ClientSubscription*)(subscriptionContext))
            p->statusChangeNotification(notification);
    }

public:
    /**
     * Constructor. Initialize the subscription with default setting.
     * @param client subscribing
     */
    ClientSubscription(Client& client);

    /**
     * Destructor
     * Only delete subscriptions from the client
     */
    virtual ~ClientSubscription();

    // Accessors
    UA_UInt32                       id()  const { return _response->subscriptionId; }
    Client&                         client()    { return _client; }
    UA_CreateSubscriptionRequest&   settings()  { return _settings; }
    UA_CreateSubscriptionResponse&  response()  { return _response; }
    
    /**
     * Hook customizing deleteSubscriptionCallback called at the end of the subscription.
     * Do nothing by default.
     */
    virtual void deleteSubscription() {}

    /**
     * Hook customizing changeNotificationCallback called when a monitored item changes.
     * Do nothing by default.
     */
    virtual void statusChangeNotification(UA_StatusChangeNotification* notification) {}

    /**
     * Create the subscription starting the monitoring process.
     * @return true if the subscription was accepted.
     */
    bool create();

    /**
     * Add a Monitored item to the subscription.
     * The same item can be added multiple time and will have a different id.
     * @warning the ids are not recycled.
     * @param item monitored
     * @return total monitored item
     */
    unsigned addMonitorItem(const MonitoredItemRef& item);

    /**
     * Remove Monitored item from the subscription.
     * @warning the ids are not recycled.
     * @param id Id of the monitored item (as returned by addMonitorItem) to delete
     */
    void deleteMonitorItem(unsigned id);

    /**
     * Find a Monitored Item by its id.
     * @param id Id of monitored item (as returned by addMonitorItem)
     * @return a pointer to the found MonitoredItem or nullptr
     */
    MonitoredItem* findMonitorItem(unsigned id);

    /**
     * Add a node as monitored item. Trigger upon node's data changing.
     * @param func a Functor to handle data change.
     * @param node to monitor
     * @return the id of the Monitored Item id
     * @see MonitoredItemDataChange
     */
    unsigned addMonitorNodeId(monitorItemFunc func, NodeId& node);

    /**
     * Add an event to trigger upon a given node's data changing.
     * @param func a functor to handle event
     * @param node id of the node to monitor
     * @param filter a selection of event filter
     * @see MonitoredItemEvent
     */
    unsigned addEventMonitor(
        monitorEventFunc    func,
        NodeId&             node,
        EventFilterSelect*  filter);
};

} // namespace Open62541

#endif // CLIENTSUBSCRIPTION_H
