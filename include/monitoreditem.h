/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef MONITOREDITEM_H
#define MONITOREDITEM_H

#include <open62541objects.h>

namespace Open62541 {

class ClientSubscription;

/**
 * This is a single monitored event.
 * Monitored events are associated (owned by) with one subscription.
 * Note the difference between Subscriptions and MonitoredItems.
 * Subscriptions are used to report back notifications.
 * MonitoredItems are used to generate notifications.
 * Every MonitoredItem is attached to exactly one Subscription.
 * And a Subscription can contain many MonitoredItems.
 */
class UA_EXPORT MonitoredItem {
    ClientSubscription&         _sub; // parent subscription

protected:
    MonitoredItemCreateResult   _response; // response
    UA_StatusCode               _lastError = 0;

    /**
     * Call-back triggered when a MonitoredItem is deleted.
     * By default only closes its subscription.
     * @param client (unused)
     * @param subId  (unused)
     * @param subContext pointer on a ClientSubscription. If null nothing is done.
     * @param monId  (unused)
     * @param monContext pointer on a MonitoredItem used to call deleteMonitoredItem()
     * @see deleteMonitoredItem()
     */
    static void deleteMonitoredItemCallback(
        UA_Client*  client,
        UA_UInt32   subId,
        void*       subContext,
        UA_UInt32   monId,
        void*       monContext);

    /**
     * Call-back for DataChange notifications.
     * Triggers when the monitored node's data changes, returning its new value.
     * @param client     (unused) pointer on the client
     * @param subId      (unused) id of the subscription
     * @param subContext pointer on a ClientSubscription. If null nothing is done.
     * @param monId      (unused) id of the monitored item
     * @param monContext pointer on a MonitoredItem used to call dataChangeNotification()
     * @param[out] outNewData pointer on the new data value.
     * @see dataChangeNotification()
     */
    static  void dataChangeNotificationCallback(
        UA_Client*      client,
        UA_UInt32       subId,
        void*           subContext,
        UA_UInt32       monId,
        void*           monContext,
        UA_DataValue*   outNewData);

    /**
     * Callback for Event notifications.
     * Triggers when the monitored node's data changes, returning the associated events.
     * @param client (unused)
     * @param subId  (unused)
     * @param subContext pointer on a ClientSubscription. If null nothing is done.
     * @param monId  (unused)
     * @param monContext pointer on a MonitoredItem used to call dataChangeNotification()
     * @param[out] eventFieldSize size of the returned array of events
     * @param[out] eventFields data of the returned array of event
     */
    static void eventNotificationCallback(
        UA_Client*  client,
        UA_UInt32   subId,
        void*       subContext,
        UA_UInt32   monId,
        void*       monContext,
        size_t      eventFieldSize,
        UA_Variant* eventFields);

public:
    /**
     * Constructor
     * @param sub owning subscription
     */
    MonitoredItem(ClientSubscription& sub)
        : _sub(sub)                     {}

    /**
     * Destructor. Cancel the subscription.
     */
    virtual ~MonitoredItem()            { remove(); }

    /**
     * @return last error code
     */
    UA_StatusCode lastError()     const { return _lastError; }

    /**
    * @return the id of the monitored event
    */
    UA_UInt32 id()                const { return _response->monitoredItemId; }

    /**
     * @return owning subscription
     */
    ClientSubscription& subscription()  { return _sub;} // parent subscription

    /**
     * Cancel the subscription
     * @return true on success
     */
    virtual bool remove();

    // Notification handlers

    /**
     * Hook to customize deleteMonitoredItemCallback(),
     * specifying additional task to do when a monitored item is deleted.
     * By default only closes its subscription.
     */
    virtual void deleteMonitoredItem()  { remove(); }

    /**
     * Hook to customize dataChangeNotificationCallback(),
     * specifying the processing of the new value.
     * @param[in, out] outNewData pointer on the new data value.
     */
    virtual void dataChangeNotification(UA_DataValue* outNewData) {}

    /**
     * Hook to customize eventNotificationCallback(),
     * specifying the processing of the events
     * triggered when the monitored node's data changes.
     * @param eventFieldSize size of the array of events
     * @param eventFields data of the array of event
     */
    virtual void eventNotification(size_t eventFieldSize, UA_Variant* eventFields) {}

protected:
    /**
     * Ask the server to change the monitoring mode.
     * The monitoring mode parameter is used to enable 
     * and disable the sampling of a MonitoredItem, 
     * and also to provide for independently enabling 
     * and disabling the reporting of Notifications.
     * This capability allows a MonitoredItem to be configured to sample, sample and report, or neither.
     * @param request specify the list of monitored items and the new monitoring mode.
     * @param[out] response with the list of monitored items for which the new monitoring mode was applied.
     * @return
     */
    bool setMonitoringMode(
        const SetMonitoringModeRequest& request,
        SetMonitoringModeResponse&      response);

    /**
     * Ask the server to switch to triggering mode.
     * The triggering mechanism is a useful feature that allows Clients
     * to reduce the data volume on the wire by configuring some items
     * to sample frequently but only report when some other Event happens.
     * @param request specify the links to add and remove
     * @param response
     * @return 
     */
    bool setTriggering(
        const SetTriggeringRequest& request,
        SetTriggeringResponse&      response);
};

/** Call-back triggered when the monitored item's data changes. */
typedef std::function<void(ClientSubscription&, UA_DataValue*)> monitorItemFunc;

/**
 * The MonitoredItemDataChange class
 * Handles value change notifications
 */
class MonitoredItemDataChange : public MonitoredItem {
    monitorItemFunc _func; /**< lambda for callback, used to process the new value.
                                must match the void (ClientSubscription&, UA_DataValue*) signature. */

public:
    /**
     * Constructor with n
     * @param sub owning subscription
     */
    MonitoredItemDataChange(ClientSubscription& sub)
        : MonitoredItem(sub) {}

    /**
     * Constructor fully defining
     * @param func a function to handle the data change notifications. Must match monitorItemFunc signature.
     * @param sub owning subscription
     */
    MonitoredItemDataChange(monitorItemFunc func, ClientSubscription& sub)
        : MonitoredItem(sub)
        , _func(func) {}

    /**
     * Change the function processing the new value.
     * @param func the new function. Must match monitorItemFunc signature.
     */
    void setFunction(monitorItemFunc func) { _func = func; }

    /**
     * Handles the new value returned when the monitored node's data changed.
     * The functor specify the handling.
     * @param[in, out] pNewData pointer on the new data value.
     */
    void dataChangeNotification(UA_DataValue* pNewData) override {
        if (_func) _func(subscription(), pNewData); // invoke functor
    }

    /**
     * Add this DataChange notification to a given node.
     * @param node id of the monitored node.
     * @param timestamp specification. Can be source, server, both (default), neither, invalid.
     * @return true on success
     */
    bool addDataChange(
        NodeId&               node,
        UA_TimestampsToReturn timestamp = UA_TIMESTAMPSTORETURN_BOTH);
};

/** Call-back handling event notifications */
typedef std::function<void(ClientSubscription&, VariantArray&)> monitorEventFunc;

/**
 * The MonitoredItemEvent class
 * Handles event notifications.
 */
class MonitoredItemEvent : public MonitoredItem {
    monitorEventFunc    _func;              /**< the event call functor */
    EventFilterSelect*  _events = nullptr;  /**< filter for events */

public:
    /**
     * Constructor with empty call-back
     * @param sub owning subscription
     */
    MonitoredItemEvent(ClientSubscription& sub)
        : MonitoredItem(sub) {}

    /**
     * Constructor
     * @param func a function to handle event notifications. Must match the monitorEventFunc signature.
     * @param sub owning subscriptions
     */
    MonitoredItemEvent(monitorEventFunc func, ClientSubscription& sub)
        : MonitoredItem(sub), _func(func) {}
    
    /**
     * Remove the subscription and delete all events.
     * @return true on success
     */
    bool remove() override; // MonitoredItem
    
    /**
     * Change the function processing the events.
     * @param func the new function. Must match the monitorEventFunc signature.
     */
    void setFunction(monitorEventFunc func) { _func = func; }

    /**
     * Handles the event notification triggered when the monitored node's data changed.
     * @param eventFieldSize size of the array of events
     * @param eventFields data of the array of event
     */
    void eventNotification(size_t nEventFields, UA_Variant* eventFields) override;

    /**
     * Add event notification to a given node.
     * @param node id of the node to modify.
     * @param events event filter.
     * @param timestamp specification. Can be source, server, both (default), neither, invalid.
     * @return true on success
     */
    bool addEvent(
        NodeId&               node,
        EventFilterSelect*    events,
        UA_TimestampsToReturn timeStamp = UA_TIMESTAMPSTORETURN_BOTH);
};

} // namespace Open62541

#endif // MONITOREDITEM_H
