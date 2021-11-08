/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated_handling.h"

namespace Open62541 {
// UA_String     -> std::string
inline std::string toString(const UA_String& str);

// non-heap allocation - no delete
// std::string      -> UA_String
UA_String toUA_String(const std::string& str);

// std::string   -> UA_String
void fromStdString(const std::string& in, UA_String& out);

// UA_ByteString -> std::string
inline std::string fromByteString(const UA_ByteString& uaByte);

// UA_Variant    -> std::string
std::string variantToString(const UA_Variant& variant);

// UA_StatusCode -> std::string
inline std::string toString(UA_StatusCode code);

// UA_DateTime   -> std::string
std::string timestampToString(UA_DateTime date);

// UA_NodeId     -> std::string
UA_EXPORT std::string toString(const UA_NodeId& node);

// UA_DataValue  -> std::string
std::string dataValueToString(const UA_DataValue& value);
}  // namespace Open62541
