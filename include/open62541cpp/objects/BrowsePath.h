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
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/objects/RelativePathElement.h>
#include <open62541cpp/objects/RelativePath.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {

/**
 * A request to translate a path into a node id. ID: 155
 * @class BrowsePath open62541objects.h
 * RAII C++ wrapper class for the UA_BrowsePath struct.
 * No getter or setter, use ->member_name to access them.
 * @see UABrowsePath in open62541.h
 */
class UA_EXPORT BrowsePath : public TypeBase<UA_BrowsePath, UA_TYPES_BROWSEPATH>
{
public:
    UA_TYPE_DEF(BrowsePath)
    BrowsePath(const NodeId& start, const RelativePath& path)
        : TypeBase(UA_BrowsePath_new())
    {
        UA_RelativePath_copy(path.constRef(), &ref()->relativePath);  // deep copy
        UA_NodeId_copy(start, &ref()->startingNode);
    }

    BrowsePath(NodeId& start, RelativePathElement& path)
        : TypeBase(UA_BrowsePath_new())
    {
        ref()->startingNode              = start.get();
        ref()->relativePath.elementsSize = 1;
        ref()->relativePath.elements     = path.ref();
    }
};
} // namespace Open62541
