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
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include "open62541/types_generated_handling.h"

namespace Open62541 {
    /**
     * A NodeId that allows the namespace URI to be specified instead of an index.
     * @class ExpandedNodeId open62541objects.h
     * RAII C++ wrapper class for the UA_ExpandedNodeId struct.
     * Setters are implemented for all member.
     * No getter, use ->member_name to access them.
     * @see UA_ExpandedNodeId in open62541.h
     */
    class UA_EXPORT ExpandedNodeId : public TypeBase<UA_ExpandedNodeId, UA_TYPES_EXPANDEDNODEID>
    {
    public:
        using TypeBase<UA_ExpandedNodeId, UA_TYPES_EXPANDEDNODEID>::operator=;

        static ExpandedNodeId ModellingRuleMandatory;

        ExpandedNodeId(const std::string namespaceUri, UA_NodeId& node, int serverIndex)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            ref()->namespaceUri = UA_STRING_ALLOC(namespaceUri.c_str());
            UA_NodeId_copy(&get().nodeId, &node);  // deep copy across
            ref()->serverIndex = serverIndex;
        }

        ExpandedNodeId(const UA_ExpandedNodeId& id)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            UA_ExpandedNodeId_copy(&id, _d.get());
        }

        UA_NodeId& nodeId() { return ref()->nodeId; }
        UA_String& namespaceUri() { return ref()->namespaceUri; }
        UA_UInt32 serverIndex() const { return get().serverIndex; }

        bool toString(std::string& s) const  // C library version of nodeid to string
        {
            UA_String o;
            UA_ExpandedNodeId_print(this->constRef(), &o);
            s = std::string((char*)o.data, o.length);
            UA_String_clear(&o);
            return true;
        }

        /* Parse the ExpandedNodeId format defined in Part 6, 5.3.1.11:
         *
         *   svr=<serverindex>;ns=<namespaceindex>;<type>=<value>
         *     or
         *   svr=<serverindex>;nsu=<uri>;<type>=<value>
         *
         * The definitions for svr, ns and nsu can be omitted and will be set to zero /
         * the empty string.*/
        bool parse(const std::string& s)
        {
            UA_String str;
            str.data   = (UA_Byte*)s.c_str();
            str.length = s.length();
            return UA_ExpandedNodeId_parse(ref(), str) == UA_STATUSCODE_GOOD;
        }

        ExpandedNodeId(const char* chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID(chars);
        }

        /** The following functions are shorthand for creating ExpandedNodeIds. */
        ExpandedNodeId(UA_UInt16 nsIndex, UA_UInt32 identifier)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_NUMERIC(nsIndex, identifier);
        }

        ExpandedNodeId(UA_UInt16 nsIndex, const std::string& chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars.c_str());
        }

        ExpandedNodeId(UA_UInt16 nsIndex, char* chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_STRING(nsIndex, chars);
        }

        ExpandedNodeId(UA_UInt16 nsIndex, const char* chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars);
        }

        ExpandedNodeId(UA_UInt16 nsIndex, UA_Guid guid)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_STRING_GUID(nsIndex, guid);
        }

        ExpandedNodeId(UA_UInt16 nsIndex, unsigned char* chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_BYTESTRING(nsIndex, (char*)chars);
        }

        ExpandedNodeId(UA_UInt16 nsIndex, const unsigned char* chars)
            : TypeBase(UA_ExpandedNodeId_new())
        {
            get() = UA_EXPANDEDNODEID_BYTESTRING_ALLOC(nsIndex, (char*)chars);
        }

        /* Does the ExpandedNodeId point to a local node? That is, are namespaceUri and
         * serverIndex empty? */
        bool isLocal() const { return UA_ExpandedNodeId_isLocal(constRef()) == UA_TRUE; }

        /* Total ordering of ExpandedNodeId */
        static UA_Order order(const UA_ExpandedNodeId* n1, const UA_ExpandedNodeId* n2)
        {
            return UA_ExpandedNodeId_order(n1, n2);
        }
        bool operator==(const ExpandedNodeId& e)
        {
            return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_EQ;
        }

        bool operator>(const ExpandedNodeId& e)
        {
            return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_MORE;
        }

        bool operator<(const ExpandedNodeId& e)
        {
            return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_LESS;
        }

        /* Returns a non-cryptographic hash for ExpandedNodeId. The hash of an
         * ExpandedNodeId is identical to the hash of the embedded (simple) NodeId if
         * the ServerIndex is zero and no NamespaceUri is set. */
        UA_UInt32 hash() const { return UA_ExpandedNodeId_hash(constRef()); }
        UA_NodeId& nodeId() { return ref()->nodeId; }
        UA_String& namespaceUri() { return ref()->namespaceUri; }
        UA_UInt32 serverIndex() { return ref()->serverIndex; }
    };
} // namespace Open62541
