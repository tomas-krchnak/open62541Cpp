
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/open62541objects.h>
#include <sstream>

namespace Open62541 {

    void EventFilterSelect::setBrowsePaths(const UAPathArray& pathArray) {
    //UAPath has all the vector stuff and can parse string paths
    if (pathArray.size()
        && pathArray.size() == _selectClause.size()) {
        for (size_t idx0 = 0; idx0 < pathArray.size(); idx0++) {
            _selectClause.setBrowsePath(idx0, pathArray[idx0]); // setup a set of browse paths
        }
    }
}

//*****************************************************************************
//*****************************************************************************

UA_StatusCode BrowserBase::browseIter(
    UA_NodeId   childId,
    UA_Boolean  isInverse, // reverse iteration (not supported)
    UA_NodeId   referenceTypeId,
    void*       handle) {

    // node iterator for browsing
    if (isInverse) {
        return UA_STATUSCODE_GOOD; // TO DO what does this do?
    }
    if (auto p = (BrowserBase*)handle) {
        p->process(childId, referenceTypeId); // process record
    }
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

void BrowserBase::print(std::ostream& os) {
    for (BrowseItem& item : _list) {
        std::string name;
        int         nsIdx;
        NodeId      node = item.nodeId; //copy i.childId, since browseName can modify it.
        if (browseName(node, name, nsIdx)) {
            os << toString(item.nodeId) << " ns:" << item.nameSpace
               << ": "  << item.name  << " Ref:"
               << toString(item.type) << std::endl;
        }
    }
}

//*****************************************************************************

BrowseItem* BrowserBase::find(const std::string& name) {

    for (auto& item : _list) {
        if (item.name == name)
            return &item;
    }
    return nullptr;
}

//*****************************************************************************

void BrowserBase::process(const UA_NodeId& node, UA_NodeId type) {
    std::string name;
    int nameSpace;
    if (browseName(node, name, nameSpace)) {
        _list.push_back(BrowseItem(name, nameSpace, node, type));
    }
}

} // namespace Open62541
