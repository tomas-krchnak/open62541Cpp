/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef NODETREETYPEDEFS_H
#define NODETREETYPEDEFS_H

#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/propertytree.h>

namespace Open62541 {
    /**
     * A thread-safe tree used to have nodes in a browsable / addressable way.
     */
    typedef NodePath<std::string> UAPath;
    typedef PropertyTree<std::string, NodeId>::PropertyNode UANode;
    typedef std::vector<UAPath> UAPathArray; /**< Events work with sets of browse paths */
    } // namespace Open62541


#endif /* NODETREETYPEDEFS_H */
