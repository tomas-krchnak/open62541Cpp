/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef ARGUMENTLIST_H
#define ARGUMENTLIST_H

#include <vector>
#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {
    // Helper containers
    class UA_EXPORT ArgumentList : public std::vector<UA_Argument>
    {
    public:
        // use constant strings for argument names - else memory leak
        //
        void addScalarArgument(const char* s, int type);
        
        // TODO add array argument types
    };
} // namespace Open62541


#endif /* ARGUMENTLIST_H */
