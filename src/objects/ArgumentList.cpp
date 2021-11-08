
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/ArgumentList.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {
    void ArgumentList::addScalarArgument(const char* name, int type)
    {
        UA_Argument item;
        UA_Argument_init(&item);
        item.description = UA_LOCALIZEDTEXT((char*)"en_US", (char*)name);
        item.name        = UA_STRING((char*)name);
        item.dataType    = UA_TYPES[type].typeId;
        item.valueRank   = -1; /* scalar */
        push_back(item);
    }
}  // namespace Open62541
