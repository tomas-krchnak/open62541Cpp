
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/ExpandedNodeId.h>

namespace Open62541 {
ExpandedNodeId ExpandedNodeId::ModellingRuleMandatory(UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY));

ExpandedNodeId::ExpandedNodeId(const std::string namespaceUri, UA_NodeId& node, int serverIndex)
    : TypeBase(UA_ExpandedNodeId_new())
{
    ref()->namespaceUri = UA_STRING_ALLOC(namespaceUri.c_str());
    UA_NodeId_copy(&get().nodeId, &node);  // deep copy across
    ref()->serverIndex = serverIndex;
}

ExpandedNodeId::ExpandedNodeId(const UA_ExpandedNodeId& id)
    : TypeBase(UA_ExpandedNodeId_new())
{
    UA_ExpandedNodeId_copy(&id, _d.get());
}

/* Returns a non-cryptographic hash for ExpandedNodeId. The hash of an
 * ExpandedNodeId is identical to the hash of the embedded (simple) NodeId if
 * the ServerIndex is zero and no NamespaceUri is set. */
UA_UInt32 ExpandedNodeId::hash() const
{
    return UA_ExpandedNodeId_hash(constRef());
}
UA_NodeId& ExpandedNodeId::nodeId()
{
    return ref()->nodeId;
}
UA_String& ExpandedNodeId::namespaceUri()
{
    return ref()->namespaceUri;
}
UA_UInt32 ExpandedNodeId::serverIndex() const
{
    return ref()->serverIndex;
}

bool ExpandedNodeId::toString(std::string& s) const  // C library version of nodeid to string
{
    UA_String o;
    UA_ExpandedNodeId_print(this->constRef(), &o);
    s = std::string((char*)o.data, o.length);
    UA_String_clear(&o);
    return true;
}

bool ExpandedNodeId::parse(const std::string& s)
{
    UA_String str;
    str.data   = (UA_Byte*)s.c_str();
    str.length = s.length();
    return UA_ExpandedNodeId_parse(ref(), str) == UA_STATUSCODE_GOOD;
}

ExpandedNodeId::ExpandedNodeId(const char* chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID(chars);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, UA_UInt32 identifier)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_NUMERIC(nsIndex, identifier);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, const std::string& chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars.c_str());
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, char* chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_STRING(nsIndex, chars);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, const char* chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, UA_Guid guid)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_STRING_GUID(nsIndex, guid);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, unsigned char* chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_BYTESTRING(nsIndex, (char*)chars);
}

ExpandedNodeId::ExpandedNodeId(UA_UInt16 nsIndex, const unsigned char* chars)
    : TypeBase(UA_ExpandedNodeId_new())
{
    get() = UA_EXPANDEDNODEID_BYTESTRING_ALLOC(nsIndex, (char*)chars);
}
bool ExpandedNodeId::isLocal() const
{
    return UA_ExpandedNodeId_isLocal(constRef()) == UA_TRUE;
}

/* Total ordering of ExpandedNodeId */
UA_Order ExpandedNodeId::order(const UA_ExpandedNodeId* n1, const UA_ExpandedNodeId* n2)
{
    return UA_ExpandedNodeId_order(n1, n2);
}
bool ExpandedNodeId::operator==(const ExpandedNodeId& e)
{
    return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_EQ;
}

bool ExpandedNodeId::operator>(const ExpandedNodeId& e)
{
    return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_MORE;
}

bool ExpandedNodeId::operator<(const ExpandedNodeId& e)
{
    return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_LESS;
}

}  // namespace Open62541
