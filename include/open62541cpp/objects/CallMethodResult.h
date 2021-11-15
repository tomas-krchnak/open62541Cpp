/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef CALLMETHODRESULT_H
#define CALLMETHODRESULT_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The result of method call request. ID: 48
     * Contains:
     * - the result status code
     * - the input argument results array
     * - the input argument diagnostic infos array
     * - the output argument array
     * @class CallMethodResult open62541objects.h
     * RAII C++ wrapper class for the UA_CallMethodResult struct.
     * No getter or setter, use ->member_name to access them.
     * @todo add accessors for the arrays
     * @see UA_CallMethodResult in open62541.h
     */
    class UA_EXPORT CallMethodResult : public TypeBase<UA_CallMethodResult, UA_TYPES_CALLMETHODRESULT>
    {
    public:
        using TypeBase<UA_CallMethodResult, UA_TYPES_CALLMETHODRESULT>::operator=;
    };
} // namespace Open62541


#endif /* CALLMETHODRESULT_H */
