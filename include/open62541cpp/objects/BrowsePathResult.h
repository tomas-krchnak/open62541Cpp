/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BROWSEPATHRESULT_H
#define BROWSEPATHRESULT_H

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/ArrayTemplate.h>

namespace Open62541 {

/**
 * The result of a translate operation. ID: 184
 * @class BrowsePathResult open62541objects.h
 * RAII C++ wrapper class for the UA_BrowsePathResult struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_BrowsePathResult in open62541.h
 */
class UA_EXPORT BrowsePathResult : public TypeBase<UA_BrowsePathResult, UA_TYPES_BROWSEPATHRESULT>
{
    static UA_BrowsePathTarget nullResult;

public:
    UA_StatusCode statusCode() const { return get().statusCode; }
    size_t targetsSize() const { return get().targetsSize; }
    UA_BrowsePathTarget target(size_t idx0) const
    {
        return (idx0 < get().targetsSize) ? get().targets[idx0] : nullResult;
    }
    BrowsePathTargetArray targets() const { return BrowsePathTargetArray(get().targets, get().targetsSize); }

    UA_BrowsePathTarget nullResult = {UA_EXPANDEDNODEID_NUMERIC(0, 0), 0};
};

} // namespace Open62541


#endif /* BROWSEPATHRESULT_H */
