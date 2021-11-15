/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef UANODEIDLIST_H
#define UANODEIDLIST_H

#include "open62541/types.h"
#include <vector>
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    /**
     * @class UANodeIdList open62541objects.h
     * RAII vector of UA_NodeId with the put method added.
     * @see UA_NodeId in open62541.h
     */
    class UA_EXPORT UANodeIdList : public std::vector<UA_NodeId>
    {
    public:
        UANodeIdList() {}
        virtual ~UANodeIdList();
        void put(const UA_NodeId& node);
    };
} // namespace Open62541


#endif /* UANODEIDLIST_H */
