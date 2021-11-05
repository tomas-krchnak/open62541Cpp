/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The result of a Create Monitored Item request. ID: 30
     * @class MonitoredItemCreateResult open62541objects.h
     * RAII C++ wrapper class for the UA_MonitoredItemCreateResult struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_MonitoredItemCreateResult in open62541.h
     */
    class UA_EXPORT MonitoredItemCreateResult
        : public TypeBase<UA_MonitoredItemCreateResult, UA_TYPES_MONITOREDITEMCREATERESULT>
    {
    public:
        using TypeBase<UA_MonitoredItemCreateResult, UA_TYPES_MONITOREDITEMCREATERESULT>::operator=;
    };
} // namespace Open62541
