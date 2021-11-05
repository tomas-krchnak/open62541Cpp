/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The attributes for a method node. ID: 143
     * @class MethodAttributes open62541objects.h
     * RAII C++ wrapper class for the UA_MethodAttributes struct.
     * Setters are implemented for 3/7 members only
     * No getter, use ->member_name to access them.
     * @todo implement all setters
     * @see UA_MethodAttributes in open62541.h
     */
    class UA_EXPORT MethodAttributes : public TypeBase<UA_MethodAttributes, UA_TYPES_METHODATTRIBUTES>
    {
    public:
        UA_TYPE_DEF(MethodAttributes)

        MethodAttributes(const std::string& name);

        auto& setDefault()
        {
            *this = UA_MethodAttributes_default;
            return *this;
        }
        auto& setDisplayName(const std::string& name)
        {
            ref()->displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.c_str());
            return *this;
        }
        auto& setDescription(const std::string& descr)
        {
            ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
            return *this;
        }
        auto& setExecutable(bool exe = true, bool user = true)
        {
            ref()->executable     = exe;
            ref()->userExecutable = user;
            return *this;
        }
    };
} // namespace Open62541
