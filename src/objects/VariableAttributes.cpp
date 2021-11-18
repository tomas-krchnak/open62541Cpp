
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include "open62541/types_generated_handling.h"
#include <open62541cpp/objects/VariableAttributes.h>

namespace Open62541 {

VariableAttributes::VariableAttributes(){};

VariableAttributes::VariableAttributes(
    const std::string& name,
    const Variant& value)
    : VariableAttributes()
{
    setDefault();
    setDisplayName(name);
    setDescription(name);
    setValue(value);
}

//*****************************************************************************

VariableAttributes& VariableAttributes::setArray(const Variant& val)
{
    const auto size = ((UA_Variant)val).arrayLength;
    const auto dim  = ((UA_Variant)val).arrayDimensionsSize;

    if (size > 0 && dim > 0) {
        // This is ok: UA_VariableAttributes.arrayDimensions own the array.
        ref()->arrayDimensions     = new UA_UInt32[1]{size};
        ref()->arrayDimensionsSize = dim;

        if (dim > 0)
            ref()->valueRank = dim;
    }
    return *this;
}

//*****************************************************************************

VariableAttributes& VariableAttributes::setHistorizing(bool isHisto /*= true*/)
{
    ref()->historizing = isHisto;

    if (isHisto)
        ref()->accessLevel |= UA_ACCESSLEVELMASK_HISTORYREAD;
    else
        ref()->accessLevel &= ~UA_ACCESSLEVELMASK_HISTORYREAD;

    return *this;
}
}  // namespace Open62541
