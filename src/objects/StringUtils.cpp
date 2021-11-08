
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
#include <boost/any.hpp>
#include "open62541/types_generated_handling.h"
#include "open62541cpp/objects/StringUtils.h"
#include <sstream>

namespace Open62541 {

/*!
\brief toString
\param r
\return UA_String as std::string
*/
inline std::string toString(UA_String& r)
{
    std::string s((const char*)(r.data), r.length);
    return s;
}
//*****************************************************************************

UA_String toUA_String(const std::string& str)
{
    UA_String uaStr;
    uaStr.length = str.size();
    uaStr.data   = (UA_Byte*)(str.c_str());
    return uaStr;
}

//*****************************************************************************

void fromStdString(const std::string& in, UA_String& out)
{
    UA_String_deleteMembers(&out);
    out = UA_STRING_ALLOC(in.c_str());
}
//*****************************************************************************

// UA_ByteString -> std::string
inline std::string fromByteString(const UA_ByteString& uaByte)
{
    return std::string((const char*)uaByte.data, uaByte.length);
}

//*****************************************************************************

std::string variantToString(const UA_Variant& v)
{
    switch (v.type->typeKind) {
        case UA_DATATYPEKIND_BOOLEAN: {  // Boolean
            return ((UA_Boolean*)(v.data)) ? "true" : "false";
        }
        case UA_DATATYPEKIND_SBYTE: {  // SByte
            int i = *((char*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_BYTE: {  // Byte
            unsigned i = *((unsigned char*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_INT16: {  // Int16
            int16_t i = *((int16_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_UINT16: {  // UInt16
            uint16_t i = *((uint16_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_INT32: {  // Int32
            int32_t i = *((int32_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_UINT32: {  // UInt32
            uint32_t i = *((uint32_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_INT64: {  // Int64
            int64_t i = *((int64_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_UINT64: {  // UInt64
            uint32_t i = *((uint32_t*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_FLOAT: {  // Float
            float i = *((float*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_DOUBLE: {  // Double
            double i = *((double*)v.data);
            return std::to_string(i);
        }
        case UA_DATATYPEKIND_STRING: {  // String
            UA_String* p = (UA_String*)(v.data);
            return std::string((const char*)p->data, p->length);
        }
        case UA_DATATYPEKIND_DATETIME: {  // DateTime
            UA_DateTime* p        = (UA_DateTime*)(v.data);
            UA_DateTimeStruct dts = UA_DateTime_toStruct(*p);
            char b[64];
            int l = sprintf(b,
                            "%02u-%02u-%04u %02u:%02u:%02u.%03u, ",
                            dts.day,
                            dts.month,
                            dts.year,
                            dts.hour,
                            dts.min,
                            dts.sec,
                            dts.milliSec);
            return std::string(b, l);
        }
        case UA_DATATYPEKIND_BYTESTRING: {  // ByteString
            UA_ByteString* p = (UA_ByteString*)(v.data);
            return std::string((const char*)p->data, p->length);
        }
        default:
            break;
    }
    return "";
}

//*****************************************************************************

// UA_StatusCode -> std::string
inline std::string toString(UA_StatusCode code)
{
    return std::string(UA_StatusCode_name(code));
}

//*****************************************************************************

std::string timestampToString(UA_DateTime date)
{
    UA_DateTimeStruct dts = UA_DateTime_toStruct(date);
    char b[64];
    int l = sprintf(b,
                    "%02u-%02u-%04u %02u:%02u:%02u.%03u, ",
                    dts.day,
                    dts.month,
                    dts.year,
                    dts.hour,
                    dts.min,
                    dts.sec,
                    dts.milliSec);
    return std::string(b, l);
}

//*****************************************************************************

std::string toString(const UA_NodeId& n)
{
    std::string ret = std::to_string(n.namespaceIndex) + ":";

    switch (n.identifierType) {
        case UA_NODEIDTYPE_NUMERIC:
            return ret + std::to_string(n.identifier.numeric);
        case UA_NODEIDTYPE_BYTESTRING:
        case UA_NODEIDTYPE_STRING:
            return ret + std::string((const char*)(n.identifier.string.data), n.identifier.string.length);
        case UA_NODEIDTYPE_GUID: {
            char buffer[45];
            int l = sprintf(buffer,
                            "%08X:%04X:%04X[%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X]",
                            n.identifier.guid.data1,
                            n.identifier.guid.data2,
                            n.identifier.guid.data3,
                            n.identifier.guid.data4[0],
                            n.identifier.guid.data4[1],
                            n.identifier.guid.data4[2],
                            n.identifier.guid.data4[3],
                            n.identifier.guid.data4[4],
                            n.identifier.guid.data4[5],
                            n.identifier.guid.data4[6],
                            n.identifier.guid.data4[7]);

            return ret + std::string(buffer, l);
        }
        default:
            break;
    }
    return std::string("Invalid Node Type");
}

//*****************************************************************************
/*!
    \brief dataValueToString
    \param value
*/
std::string dataValueToString(const UA_DataValue& value)
{
    std::stringstream os;
    os << "ServerTime:" << timestampToString(value.serverTimestamp) << " ";
    os << "SourceTime:" << timestampToString(value.sourceTimestamp) << " ";
    os << "Status:" << std::hex << value.status << " ";
    os << "Value:" << variantToString(value.value);
    return os.str();
}
//*****************************************************************************


}  // namespace Open62541
