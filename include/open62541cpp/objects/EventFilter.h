/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * \brief setClause
     * @class EventFilter open62541objects.h
     * RAII C++ wrapper class for the UA_EventFilter struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_EventFilter in open62541.h
     */
    class EventFilter : public TypeBase<UA_EventFilter, UA_TYPES_EVENTFILTER>
    {
    public:
        using TypeBase<UA_EventFilter, UA_TYPES_EVENTFILTER>::operator=;
    };
} // namespace Open62541


#endif /* EVENTFILTER_H */
