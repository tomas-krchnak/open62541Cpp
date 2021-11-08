
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include "open62541/nodeids.h"
#include <open62541cpp/objects/EventSelectClauseArray.h>
#include <open62541cpp/objects/NodeTreeTypeDefs.h>

namespace Open62541 {

EventSelectClauseArray::EventSelectClauseArray(size_t size)
    : SimpleAttributeOperandArray(size)
{
    for (size_t idx0 = 0; idx0 < size; idx0++) {
        at(idx0).attributeId      = UA_ATTRIBUTEID_VALUE;
        at(idx0).typeDefinitionId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE);
    }
}

//*****************************************************************************

void EventSelectClauseArray::setBrowsePath(size_t idx0, const UAPath& path)
{
    if (idx0 < length()) {
        // allocate array
        QualifiedNameArray bp(path.size());
        // set from the path
        for (size_t j = 0; j < bp.length(); j++) {
            // populate
            const std::string& s = path[j];
            bp.at(j)             = UA_QUALIFIEDNAME_ALLOC(0, s.c_str());
        }

        at(idx0).browsePath     = bp.data();
        at(idx0).browsePathSize = bp.length();
        bp.release();
    }
}

//*****************************************************************************

void EventSelectClauseArray::setBrowsePath(size_t idx0, const std::string& fullPath)
{
    UAPath path;
    path.toList(fullPath);
    setBrowsePath(idx0, path);
}
}  // namespace Open62541
