/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/EventSelectClauseArray.h>
#include <open62541cpp/objects/EventFilter.h>
#include <open62541cpp/propertytree.h>

namespace Open62541 {

    typedef NodePath<std::string> UAPath;
    typedef std::vector<UAPath> UAPathArray; /**< Events work with sets of browse paths */

    class UA_EXPORT EventFilterSelect : public EventFilter
    {
        EventSelectClauseArray _selectClause;  // these must have the life time of the monitored event

    public:
        EventFilterSelect() = default;
        EventFilterSelect(size_t size)
            : _selectClause(size)
        {
        }
        ~EventFilterSelect() { _selectClause.clear(); }
        EventSelectClauseArray& selectClause() { return _selectClause; }
        void setBrowsePaths(const UAPathArray& pathArray);
    };
} // namespace Open62541
