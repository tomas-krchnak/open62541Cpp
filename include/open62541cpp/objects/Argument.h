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
     * An argument for a method. ID: 132
     * @class ArgumentList open62541objects.h
     * RAII C++ wrapper class for the UA_Argument struct.
     * Setters are implemented for all members,
     * except arrayDimensionsSize and arrayDimensions.
     * No getter, use ->member_name to access them.
     * @see UA_Argument in open62541.h
     */
    class UA_EXPORT Argument : public TypeBase<UA_Argument, UA_TYPES_ARGUMENT>
    {
    public:
        auto& setDataType(int idx0)
        {
            ref()->dataType = UA_TYPES[idx0].typeId;
            return *this;
        }
        auto& setDescription(const std::string& descr)
        {
            ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
            return *this;
        }
        auto& setName(const std::string& name)
        {
            ref()->name = UA_STRING_ALLOC(name.c_str());
            return *this;
        }
        auto& setValueRank(int rank)
        {
            ref()->valueRank = rank;
            return *this;
        }

        UA_Argument& set(int type, const std::string& name, const std::string& desc = "", int rank = -1)
        {
            setDataType(type);
            setDescription(name);
            setName(desc);
            setValueRank(rank);
            return get();
        }
    };
} // namespace Open62541
