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
     * The attributes for a data type node. ID: 93
     * @class DataTypeAttributes open62541objects.h
     * RAII C++ wrapper class for the UA_DataTypeAttributes struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_DataTypeAttributes in open62541.h
     */
    class UA_EXPORT DataTypeAttributes : public TypeBase<UA_DataTypeAttributes, UA_TYPES_DATATYPEATTRIBUTES>
    {
    public:
        UA_TYPE_DEF(DataTypeAttributes)
        auto& setDefault()
        {
            *this = UA_DataTypeAttributes_default;
            return *this;
        }
    };
} // namespace Open62541
