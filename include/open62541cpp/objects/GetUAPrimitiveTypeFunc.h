/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef GETUAPRIMITIVETYPEFUNC_H
#define GETUAPRIMITIVETYPEFUNC_H
#include "open62541/types.h"
#include "open62541/types_generated.h"

namespace Open62541 {

    inline const UA_DataType* GetUAPrimitiveType(bool)               { return &UA_TYPES[UA_TYPES_BOOLEAN]; }
    inline const UA_DataType* GetUAPrimitiveType(byte)               { return &UA_TYPES[UA_TYPES_BYTE]; }
    inline const UA_DataType* GetUAPrimitiveType(short)              { return &UA_TYPES[UA_TYPES_INT16]; }
    inline const UA_DataType* GetUAPrimitiveType(unsigned short)     { return &UA_TYPES[UA_TYPES_UINT16]; }
    inline const UA_DataType* GetUAPrimitiveType(int)                { return &UA_TYPES[UA_TYPES_INT32]; }
    inline const UA_DataType* GetUAPrimitiveType(unsigned)           { return &UA_TYPES[UA_TYPES_UINT32]; }
    inline const UA_DataType* GetUAPrimitiveType(long)               { return &UA_TYPES[UA_TYPES_INT32]; }
    inline const UA_DataType* GetUAPrimitiveType(unsigned long)      { return &UA_TYPES[UA_TYPES_UINT32]; }
    inline const UA_DataType* GetUAPrimitiveType(float)              { return &UA_TYPES[UA_TYPES_FLOAT]; }
    inline const UA_DataType* GetUAPrimitiveType(double)             { return &UA_TYPES[UA_TYPES_DOUBLE]; }
    inline const UA_DataType* GetUAPrimitiveType(const UA_String&)   { return &UA_TYPES[UA_TYPES_STRING]; }
    inline const UA_DataType* GetUAPrimitiveType(UA_DateTime)        { return &UA_TYPES[UA_TYPES_DATETIME]; }

} // namespace Open62541


#endif /* GETUAPRIMITIVETYPEFUNC_H */
