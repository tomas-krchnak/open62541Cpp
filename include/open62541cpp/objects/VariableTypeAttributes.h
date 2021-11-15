/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef VARIABLETYPEATTRIBUTES_H
#define VARIABLETYPEATTRIBUTES_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * The attributes for a variable type node. ID: 47
     * @class VariableTypeAttributes open62541objects.h
     * RAII C++ wrapper class for the UA_VariableTypeAttributes struct.
     * Setters are implemented for 2/11 members only
     * No getter, use ->member_name to access them.
     * @todo implement all setters
     * @see UA_VariableTypeAttributes in open62541.h
     */
class UA_EXPORT VariableTypeAttributes : public TypeBase<UA_VariableTypeAttributes, UA_TYPES_VARIABLETYPEATTRIBUTES>
    {
    public:
         using TypeBase<UA_VariableTypeAttributes, UA_TYPES_VARIABLETYPEATTRIBUTES>::operator=;
        auto& setDefault()
        {
            *this = UA_VariableTypeAttributes_default;
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
    };

} // namespace Open62541


#endif /* VARIABLETYPEATTRIBUTES_H */
