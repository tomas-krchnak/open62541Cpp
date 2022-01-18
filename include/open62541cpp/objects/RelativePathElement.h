/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef RELATIVEPATHELEMENT_H
#define RELATIVEPATHELEMENT_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/QualifiedName.h>

namespace Open62541 {
    /**
     * An element in a relative path. ID: 86
     * @class RelativePathElement open62541objects.h
     * RAII C++ wrapper class for the UA_RelativePathElement struct.
     * Setters are implemented for all members,
     * except arrayDimensionsSize and arrayDimensions.
     * No getter, use ->member_name to access them.
     * @see UA_RelativePathElement in open62541.h
     */
    class RelativePathElement : public TypeBase<UA_RelativePathElement, UA_TYPES_RELATIVEPATHELEMENT>
    {
    public:
        RelativePathElement(QualifiedName& item, NodeId& typeId, bool inverse = false, bool includeSubTypes = false)
            : TypeBase(UA_RelativePathElement_new())
        {
            ref()->referenceTypeId = typeId.get();
            ref()->isInverse       = includeSubTypes;
            ref()->includeSubtypes = inverse;
            ref()->targetName      = item.get();  // shallow copy!!!
        }
    };
} // namespace Open62541


#endif /* RELATIVEPATHELEMENT_H */
