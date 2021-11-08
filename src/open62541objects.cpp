
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

//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
//*****************************************************************************

NodeIdMap::~NodeIdMap() {
    for (auto& i : *this) {
        UA_NodeId_deleteMembers(&i.second); // delete node data
    }
    clear();
}

//*****************************************************************************

void NodeIdMap::put(const UA_NodeId& node) {
    UA_NodeId copy; // deep copy
    UA_NodeId_init(&copy);
    UA_NodeId_copy(&node, &copy);
    const std::string s = toString(copy);
    insert(std::pair<std::string, UA_NodeId>(s, copy));
}

//*****************************************************************************
//*****************************************************************************

void ArgumentList::addScalarArgument(const char* name, int type) {
    UA_Argument item;
    UA_Argument_init(&item);
    item.description = UA_LOCALIZEDTEXT((char*)"en_US", (char*)name);
    item.name = UA_STRING((char*)name);
    item.dataType = UA_TYPES[type].typeId;
    item.valueRank = -1; /* scalar */
    push_back(item);
}

//*****************************************************************************
//*****************************************************************************

ObjectAttributes::ObjectAttributes(const std::string& name)
    : ObjectAttributes() {
    setDefault();
    setDisplayName(name);
    setDescription(name);
}

//*****************************************************************************
//*****************************************************************************

MethodAttributes::MethodAttributes(const std::string& name)
    : MethodAttributes() {
    setDefault();
    setDisplayName(name);
    setDescription(name);
}

//*****************************************************************************
//*****************************************************************************

VariableAttributes::VariableAttributes(
    const std::string&  name,
    const Variant&      value)
    : VariableAttributes() {
    setDefault();
    setDisplayName(name);
    setDescription(name);
    setValue(value);
}

//*****************************************************************************

VariableAttributes& VariableAttributes::setArray(const Variant& val) {
    const auto size = val->arrayLength;
    const auto dim  = val->arrayDimensionsSize;

    if (size > 0 && dim > 0) {
        // This is ok: UA_VariableAttributes.arrayDimensions own the array.
        ref()->arrayDimensions     = new UA_UInt32[1]{ size };
        ref()->arrayDimensionsSize = dim;

        if (dim > 0)
            ref()->valueRank = dim;
    }
    return *this;
}

//*****************************************************************************

VariableAttributes& VariableAttributes::setHistorizing(bool isHisto /*= true*/) {
    ref()->historizing = isHisto;

    if (isHisto)
        ref()->accessLevel |= UA_ACCESSLEVELMASK_HISTORYREAD;
    else
        ref()->accessLevel &= ~UA_ACCESSLEVELMASK_HISTORYREAD;

    return *this;
}

//*****************************************************************************
//*****************************************************************************

bool UANodeTree::createPathFolders(
    const UAPath& path,
    UANode*       pNode,
    int           level /*= 0*/) {
    bool ret = false;
    if (!pNode) return ret;

    if (!pNode->hasChild(path[level])) {
        NodeId no;
        ret = addFolderNode(pNode->data(), path[level], no);
        if (ret) {
            auto nn = pNode->add(path[level]);
            if (nn) nn->setData(no);
        }
    }

    // recurse
    auto pChild = pNode->child(path[level]);
    level++;
    if (level < int(path.size())) {
        ret = createPathFolders(path, pChild, level);
    }

    return ret;
}

//*****************************************************************************

bool UANodeTree::createPath(
    const UAPath&   path,
    UANode*         pNode,
    const Variant&  val,
    int             level /*= 0*/) {
    bool ret = false;
    if (!pNode) return ret;

    if (!pNode->hasChild(path[level])) {
        if (level == int(path.size() - 1)) { // terminal node , hence value
            NodeId no;
            ret = addValueNode(pNode->data(), path[level], val, no);
            if (ret) {
                if (auto nn = pNode->add(path[level]))
                    nn->setData(no);
            }
        }
        else {
            NodeId no;
            ret = addFolderNode(pNode->data(), path[level], no);
            if (ret) {
                auto nn = pNode->add(path[level]);
                if (nn) nn->setData(no);
            }
        }
    }

    // recurse
    auto pChild = pNode->child(path[level]);
    level++;
    if (level < int(path.size())) {
        ret = createPath(path, pChild, val, level);
    }

    return ret;
}

//*****************************************************************************

bool UANodeTree::setNodeValue(const UAPath& path, const Variant& val) {
    if (exists(path)) {
        return setValue(node(path)->data(), val); // easy
    }
    else if (path.size() > 0) {
        // create the path and add nodes as needed
        if (createPath(path, rootNode(), val)) {
            return setValue(node(path)->data(), val);
        }
    }
    return false;
}

//*****************************************************************************

bool UANodeTree::setNodeValue(
    UAPath              path,
    const std::string&  child,
    const Variant&      val) {
    path.push_back(child);
    bool ret = setNodeValue(path, val);
    path.pop_back();
    return ret;
}

//*****************************************************************************

bool UANodeTree::getNodeValue(const UAPath& path, Variant& outValue) {
    outValue.null();
    UANode* np = node(path);
    if (np) { // path exist ?
        return getValue(np->data(), outValue);
    }
    return false;
}

//*****************************************************************************

bool UANodeTree::getNodeValue(UAPath path, const std::string& name, Variant& outValue) {
    path.push_back(name);
    bool ret = getNodeValue(path, outValue);
    path.pop_back();
    return ret;
}

//*****************************************************************************

void UANodeTree::printNode(const UANode* pNode, std::ostream& os, int level /*= 0*/) {
    if (!pNode) {
        return; // no node to print
    }
    std::string indent(level, ' ');
    os << indent << pNode->name();
    os << toString(pNode->constData());
    os << std::endl;

    if (pNode->totalChildren() < 1) {
        return; // no children node to print
    }
    level++;
    for (const auto& child : pNode->constChildren()) {
        printNode(child.second, os, level); // recurse
    }
}

//*****************************************************************************
//*****************************************************************************

EventSelectClauseArray::EventSelectClauseArray(size_t size)
    : SimpleAttributeOperandArray(size) {
    for (size_t idx0 = 0; idx0 < size; idx0++) {
        at(idx0).attributeId = UA_ATTRIBUTEID_VALUE;
        at(idx0).typeDefinitionId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE);
    }
}

//*****************************************************************************

void EventSelectClauseArray::setBrowsePath(size_t idx0, const UAPath& path) {
    if (idx0 < size()) {
        // allocate array
        QualifiedNameArray bp(path.size());
        // set from the path
        for (size_t j = 0; j < bp.size(); j++) {
            // populate
            const std::string& s = path[j];
            bp.at(j) = UA_QUALIFIEDNAME_ALLOC(0, s.c_str());
        }

        at(idx0).browsePath = bp.data();
        at(idx0).browsePathSize = bp.size();
        bp.release();
    }
}

//*****************************************************************************

void EventSelectClauseArray::setBrowsePath(size_t idx0, const std::string& fullPath) {
    UAPath path;
    path.toList(fullPath);
    setBrowsePath(idx0, path);
}

//*****************************************************************************
//*****************************************************************************

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
