/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef SETTRIGGERINGREQUEST_H
#define SETTRIGGERINGREQUEST_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {

    /**
     * Request to Set Triggering. ID: 173
     * @class SetTriggeringRequest open62541objects.h
     * RAII C++ wrapper class for the UA_SetTriggeringRequest struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_SetTriggeringRequest in open62541.h
     */
    class SetTriggeringRequest : public TypeBase<UA_SetTriggeringRequest, UA_TYPES_SETTRIGGERINGREQUEST>
    {
    public:
        using TypeBase<UA_SetTriggeringRequest, UA_TYPES_SETTRIGGERINGREQUEST>::operator=;
    };
} // namespace Open62541


#endif /* SETTRIGGERINGREQUEST_H */
