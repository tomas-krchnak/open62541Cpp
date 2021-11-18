/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#ifndef OPEN62541TYPEDEFS_H
#define OPEN62541TYPEDEFS_H

#include "open62541/types_generated.h"

#include <string>
#include <vector>

namespace Open62541 {

/**
 * A mask specifying the class of the node.
 * Alias for UA_NodeClass
 * @see UA_NodeClass in open62541.h
 */
typedef UA_NodeClass NodeClass;

typedef std::vector<std::string> Path;

/*!
    \brief VariantList
*/
typedef std::vector<UA_Variant> VariantList;  // shallow copied

} // namespace Open62541
#endif /* OPEN62541TYPEDEFS_H */
