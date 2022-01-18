/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef CREATESUBSCRIPTIONREQUEST_H
#define CREATESUBSCRIPTIONREQUEST_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {

    // Request / Response wrappers for monitored items and events
    /**
     * A request to create a subscription. ID: 162
     * @class CreateSubscriptionRequest open62541objects.h
     * RAII C++ wrapper class for the UA_CreateSubscriptionRequest struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_CreateSubscriptionRequest in open62541.h
     */
    class CreateSubscriptionRequest
        : public TypeBase<UA_CreateSubscriptionRequest, UA_TYPES_CREATESUBSCRIPTIONREQUEST>
    {
    public:
        using TypeBase<UA_CreateSubscriptionRequest, UA_TYPES_CREATESUBSCRIPTIONREQUEST>::operator=;
    };
} // namespace Open62541


#endif /* CREATESUBSCRIPTIONREQUEST_H */
