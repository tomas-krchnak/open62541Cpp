/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef MONITOREDITEMCREATEREQUEST_H
#define MONITOREDITEMCREATEREQUEST_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/NodeId.h>

namespace Open62541 {
    /**
     * Request to create a monitored item. ID: 128
     * @class MonitoredItemCreateRequest open62541objects.h
     * RAII C++ wrapper class for the UA_MonitoredItemCreateRequest struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_MonitoredItemCreateRequest in open62541.h
     * @warning not to be confused with CreateMonitoredItemsRequest
     */
    class UA_EXPORT MonitoredItemCreateRequest
        : public TypeBase<UA_MonitoredItemCreateRequest, UA_TYPES_MONITOREDITEMCREATEREQUEST>
    {
    public:
        using TypeBase<UA_MonitoredItemCreateRequest, UA_TYPES_MONITOREDITEMCREATEREQUEST>::operator=;
        void setItem(const NodeId& nodeId,
                     UA_UInt32 attributeId            = UA_ATTRIBUTEID_EVENTNOTIFIER,
                     UA_MonitoringMode monitoringMode = UA_MONITORINGMODE_REPORTING)
        {
            get().itemToMonitor.nodeId      = nodeId;
            get().monitoringMode            = monitoringMode;
            get().itemToMonitor.attributeId = attributeId;
        }
        void setFilter(UA_EventFilter* filter,
                       UA_ExtensionObjectEncoding encoding = UA_EXTENSIONOBJECT_DECODED,
                       const UA_DataType* type             = &UA_TYPES[UA_TYPES_EVENTFILTER])
        {
            get().requestedParameters.filter.encoding             = encoding;
            get().requestedParameters.filter.content.decoded.data = filter;
            get().requestedParameters.filter.content.decoded.type = type;
        }

        UA_EventFilter* filter() { return (UA_EventFilter*)(get().requestedParameters.filter.content.decoded.data); }
    };
} // namespace Open62541


#endif /* MONITOREDITEMCREATEREQUEST_H */
