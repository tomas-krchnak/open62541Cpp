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

typedef std::function<void (ClientSubscription&, UA_DataValue*)> monitorItemFunc;   /**< Callback for a (data change) monitored item */
typedef std::function<void (ClientSubscription&, VariantArray&)> monitorEventFunc;  /**< call back for an event */

/**
 * The MonitoredItem class
 * This is a single monitored event. Monitored events are associated (owned) by subscriptions
 */
class UA_EXPORT MonitoredItem {
    ClientSubscription &_sub; // parent subscription
protected:
    MonitoredItemCreateResult _response; // response
    UA_StatusCode _lastError = 0;

    /**
     * Callback for the deletion of a MonitoredItem 
     * @param client
     * @param subId
     * @param subContext
     * @param monId
     * @param monContext
     */
    static void deleteMonitoredItemCallback(
        UA_Client*  client,
        UA_UInt32   subId,
        void*       subContext,
        UA_UInt32   monId,
        void*       monContext);

    /**
     * Callback for DataChange notifications
     * @param client
     * @param subId
     * @param subContext
     * @param monId
     * @param monContext
     * @param value
     */
    static  void dataChangeNotificationCallback(
        UA_Client*      client,
        UA_UInt32       subId,
        void*           subContext,
        UA_UInt32       monId,
        void*           monContext,
        UA_DataValue*   value);

    /**
     * Callback for Event notifications
     * @param client
     * @param subId
     * @param subContext
     * @param monId
     * @param monContext
     * @param nEventFields
     * @param eventFields
     */
    static void eventNotificationCallback(
        UA_Client*  client,
        UA_UInt32   subId,
        void*       subContext,
        UA_UInt32   monId,
        void*       monContext,
        size_t      nEventFields,
        UA_Variant* eventFields);

public:
    /**
     * MonitoredItem
     * @param sub owning subscription
     */
    MonitoredItem(ClientSubscription& sub);

    /**
     * ~MonitoredItem
     */
    virtual ~MonitoredItem() {
        remove();
    }

    /**
     * lastError
     * @return last error code
     */
    UA_StatusCode  lastError() const {
        return _lastError;
    }

    /**
     * subscription
     * @return owning subscription
     */
    ClientSubscription& subscription() { return _sub;} // parent subscription

    // Notification handlers

    /**
     * deleteMonitoredItem
     */
    virtual void deleteMonitoredItem() { remove(); }

    /**
     * dataChangeNotification
     * @param value
     */
    virtual void dataChangeNotification(UA_DataValue* value) {}

    /**
     * eventNotification
     * @param nEventFields
     * @param eventFields
     */
    virtual void eventNotification(size_t nEventFields, UA_Variant* eventFields) {}

    /**
     * remove
     * @return true on success
     */
    virtual bool remove();

    /**
     * id
     * @return the id of the monitored event
     */
    UA_UInt32 id() {
        return _response.get().monitoredItemId;
    }

protected:
    /**
     * setMonitoringMode
     * @param request
     * @param response
     * @return
     */
    bool setMonitoringMode(
        const SetMonitoringModeRequest& request,
        SetMonitoringModeResponse&      response);

    /**
     * setTriggering
     * @param request
     * @param request
     * @return 
     */
    bool setTriggering(
        const SetTriggeringRequest& request,
        SetTriggeringResponse&      response);
};

/**
 * The MonitoredItemDataChange class
 * Handles value change notifications
 */
class MonitoredItemDataChange : public MonitoredItem {
    monitorItemFunc _func; /**< lambda for callback */

public:
    /**
     * MonitoredItem
     * @param sub owning subscription
     */
    MonitoredItemDataChange(ClientSubscription& sub)
        : MonitoredItem(sub) {}

    /**
     * MonitoredItem
     * @param func a functor to handle notifications
     * @param sub owning subscription
     */
    MonitoredItemDataChange(monitorItemFunc func, ClientSubscription& sub)
        : MonitoredItem(sub), _func(func) {}

    /**
     * setFunction
     * @param func functor
     */
    void setFunction(monitorItemFunc func) {
        _func = func;
    }

    /**
     * dataChangeNotification
     * @param value new value
     */
    virtual void dataChangeNotification(UA_DataValue* value) {
        if (_func) _func(subscription(), value); // invoke functor
    }

    /**
     * addDataChange
     * @param node id
     * @param ts timestamp specification
     * @return true on success
     */
    bool addDataChange(
        NodeId&               node,
        UA_TimestampsToReturn ts = UA_TIMESTAMPSTORETURN_BOTH);
};

/**
 * The MonitoredItemEvent class
 */
class MonitoredItemEvent : public MonitoredItem {
    monitorEventFunc _func;                 /**< the event call functor */
    EventFilterSelect * _events = nullptr;  /**< filter for events */

public:
    /**
     * MonitoredItem
     * @param sub owning subscription
     */
    MonitoredItemEvent(ClientSubscription& sub)
        : MonitoredItem(sub) {}

    /**
     * MonitoredItem
     * @param func a functor to handle event notifications
     * @param sub owning subscriptions
     */
    MonitoredItemEvent(monitorEventFunc func, ClientSubscription& sub)
        : MonitoredItem(sub), _func(func) {}
    
    /**
     * remove
     * @return true on success
     */
    bool remove()
    {
        bool ret = MonitoredItem::remove();
        if(_events) delete _events;
        return ret;
    }
    
    /**
     * setFunction
     * @param f functor
     */
    void setFunction(monitorEventFunc func) {
        _func = func;
    }

    /**
     * eventNotification
     * Handles the event notification
     */
    virtual void eventNotification(size_t nEventFields, UA_Variant* eventFields) {
        if (_func) {
            VariantArray va;
            va.setList(nEventFields, eventFields);
            _func(subscription(), va); // invoke functor
            va.release();
        }
    }

    /**
     * addEvent
     * @param node id
     * @param events event filter
     * @param ts timestamp flags
     * @return true on success
     */
    bool addEvent(
        NodeId&               node,
        EventFilterSelect*    events,
        UA_TimestampsToReturn ts = UA_TIMESTAMPSTORETURN_BOTH);
};

} // namespace Open62541

#endif // MONITOREDITEM_H
