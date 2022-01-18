/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef QUALIFIEDNAME_H
#define QUALIFIEDNAME_H

#include <string>
#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {

/**
 * A name qualified by a namespace.
 * @class QualifiedName open62541objects.h
 * RAII C++ wrapper class for the UA_QualifiedName struct.
 * Setters are implemented for all member.
 * @see UA_QualifiedName in open62541.h
 */
class QualifiedName : public TypeBase<UA_QualifiedName, UA_TYPES_QUALIFIEDNAME>
{
public:
    QualifiedName() = default;

    QualifiedName(int ns, const char* str)
        : TypeBase(UA_QualifiedName_new())
    {
        *ref() = UA_QUALIFIEDNAME_ALLOC(ns, str);
    }

    QualifiedName(int ns, const std::string& str)
        : TypeBase(UA_QualifiedName_new())
    {
        *ref() = UA_QUALIFIEDNAME_ALLOC(ns, str.c_str());
    }

    UA_UInt16 namespaceIndex() const { return get().namespaceIndex; }
    UA_String& name() { return ref()->name; }
};


} // namespace Open62541


#endif /* QUALIFIEDNAME_H */
