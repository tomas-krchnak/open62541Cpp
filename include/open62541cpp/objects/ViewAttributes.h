/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef VIEWATTRIBUTES_H
#define VIEWATTRIBUTES_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The attributes for a view node. ID: 25
     * @class ViewAttributes open62541objects.h
     * RAII C++ wrapper class for the UA_ViewAttributes struct.
     * No getter or setter, use ->member_name to access them.
     * @see UA_ViewAttributes in open62541.h
     */
    class ViewAttributes : public TypeBase<UA_ViewAttributes, UA_TYPES_VIEWATTRIBUTES>
    {
    public:
        using TypeBase<UA_ViewAttributes, UA_TYPES_VIEWATTRIBUTES>::operator=;
        auto& setDefault()
        {
            *this = UA_ViewAttributes_default;
            return *this;
        }
    };
} // namespace Open62541


#endif /* VIEWATTRIBUTES_H */
