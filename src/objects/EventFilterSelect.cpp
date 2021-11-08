
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/NodeTreeTypeDefs.h>
#include <open62541cpp/objects/EventFilterSelect.h>

namespace Open62541 {
void EventFilterSelect::setBrowsePaths(const UAPathArray& pathArray)
{
    // UAPath has all the vector stuff and can parse string paths
    if (pathArray.size() && pathArray.size() == _selectClause.length()) {
        for (size_t idx0 = 0; idx0 < pathArray.size(); idx0++) {
            _selectClause.setBrowsePath(idx0, pathArray[idx0]);  // setup a set of browse paths
        }
    }
}
}  // namespace Open62541
