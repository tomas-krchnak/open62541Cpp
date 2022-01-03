/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated_handling.h"

namespace Open62541 {

    /*!
\brief toString
\param r
\return UA_String as std::string
*/
UA_EXPORT inline std::string toString(UA_String& r)
{
    std::string s((const char*)(r.data), r.length);
    return s;
}

UA_EXPORT inline std::string toString(const UA_String& r)
{
    std::string s((const char*)(r.data), r.length);
    return s;
}

//*****************************************************************************

// UA_ByteString -> std::string
UA_EXPORT inline std::string fromByteString(const UA_ByteString& uaByte)
{
    return std::string((const char*)uaByte.data, uaByte.length);
}

//*****************************************************************************

// UA_StatusCode -> std::string
UA_EXPORT inline std::string toString(UA_StatusCode code)
{
    return std::string(UA_StatusCode_name(code));
}

// non-heap allocation - no delete
// std::string      -> UA_String
UA_EXPORT UA_String toUA_String(const std::string& str);

// std::string   -> UA_String
UA_EXPORT void fromStdString(const std::string& in, UA_String& out);

// UA_Variant    -> std::string
UA_EXPORT std::string variantToString(const UA_Variant& variant);

// UA_DateTime   -> std::string
UA_EXPORT std::string timestampToString(UA_DateTime date);

// UA_NodeId     -> std::string
UA_EXPORT std::string toString(const UA_NodeId& node);

// UA_DataValue  -> std::string
UA_EXPORT std::string dataValueToString(const UA_DataValue& value);
}  // namespace Open62541


#endif /* STRINGUTILS_H */
