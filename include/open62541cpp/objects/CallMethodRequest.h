/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef CALLMETHODREQUEST_H
#define CALLMETHODREQUEST_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * A request to call a method. ID: 61
     * Contains the node of the method to call and the argument list + size.
     * @class CallMethodRequest open62541objects.h
     * RAII C++ wrapper class for the UA_CallMethodRequest struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_CallMethodRequest in open62541.h
     */
class UA_EXPORT CallMethodRequest : public TypeBase<UA_CallMethodRequest, UA_TYPES_CALLMETHODREQUEST>
    {
    public:
    };
} // namespace Open62541


#endif /* CALLMETHODREQUEST_H */
