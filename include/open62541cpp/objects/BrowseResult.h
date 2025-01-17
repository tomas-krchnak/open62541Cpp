/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BROWSERESULT_H
#define BROWSERESULT_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The result of a browse operation. ID: 174
     * @class BrowseResult open62541objects.h
     * RAII C++ wrapper class for the UA_BrowseResult struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_BrowseResult in open62541.h
     */
    class BrowseResult : public TypeBase<UA_BrowseResult, UA_TYPES_BROWSERESULT>
    {
    public:
    };
} // namespace Open62541


#endif /* BROWSERESULT_H */
