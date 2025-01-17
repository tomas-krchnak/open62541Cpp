/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef CREATEMONITOREDITEMSREQUEST_H
#define CREATEMONITOREDITEMSREQUEST_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * @class CreateMonitoredItemsRequest open62541objects.h
     * RAII C++ wrapper class for the UA_CreateMonitoredItemsRequest struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_CreateMonitoredItemsRequest in open62541.h
     * @warning not to be confused with MonitoredItemCreateRequest
     */
    class CreateMonitoredItemsRequest
        : public TypeBase<UA_CreateMonitoredItemsRequest, UA_TYPES_CREATEMONITOREDITEMSREQUEST>
    {
    public:
    };
} // namespace Open62541


#endif /* CREATEMONITOREDITEMSREQUEST_H */
