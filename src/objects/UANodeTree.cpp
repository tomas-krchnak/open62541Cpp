
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/UANodeTree.h>
#include <open62541cpp/objects/Variant.h>

namespace Open62541 {

bool UANodeTree::createPathFolders(const UAPath& path, UANode* pNode, int level /*= 0*/)
{
    bool ret = false;
    if (!pNode)
        return ret;

    if (!pNode->hasChild(path[level])) {
        NodeId no;
        ret = addFolderNode(pNode->data(), path[level], no);
        if (ret) {
            auto nn = pNode->add(path[level]);
            if (nn)
                nn->setData(no);
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

bool UANodeTree::createPath(const UAPath& path, UANode* pNode, const Variant& val, int level /*= 0*/)
{
    bool ret = false;
    if (!pNode)
        return ret;

    if (!pNode->hasChild(path[level])) {
        if (level == int(path.size() - 1)) {  // terminal node , hence value
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
                if (nn)
                    nn->setData(no);
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

bool UANodeTree::setNodeValue(const UAPath& path, const Variant& val)
{
    if (exists(path)) {
        return setValue(node(path)->data(), val);  // easy
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

bool UANodeTree::setNodeValue(UAPath path, const std::string& child, const Variant& val)
{
    path.push_back(child);
    bool ret = setNodeValue(path, val);
    path.pop_back();
    return ret;
}

//*****************************************************************************

bool UANodeTree::getNodeValue(const UAPath& path, Variant& outValue)
{
    outValue.null();
    UANode* np = node(path);
    if (np) {  // path exist ?
        return getValue(np->data(), outValue);
    }
    return false;
}

//*****************************************************************************

bool UANodeTree::getNodeValue(UAPath path, const std::string& name, Variant& outValue)
{
    path.push_back(name);
    bool ret = getNodeValue(path, outValue);
    path.pop_back();
    return ret;
}

//*****************************************************************************

void UANodeTree::printNode(const UANode* pNode, std::ostream& os, int level /*= 0*/)
{
    if (!pNode) {
        return;  // no node to print
    }
    std::string indent(level, ' ');
    os << indent << pNode->name();
    os << toString(pNode->constData());
    os << std::endl;

    if (pNode->totalChildren() < 1) {
        return;  // no children node to print
    }
    level++;
    for (const auto& child : pNode->constChildren()) {
        printNode(child.second, os, level);  // recurse
    }
}
}  // namespace Open62541
