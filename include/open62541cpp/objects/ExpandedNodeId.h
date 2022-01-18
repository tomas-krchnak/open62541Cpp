/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef EXPANDEDNODEID_H
#define EXPANDEDNODEID_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include "open62541/types_generated_handling.h"
#include "open62541/nodeids.h"

namespace Open62541 {
    /**
     * A NodeId that allows the namespace URI to be specified instead of an index.
     * @class ExpandedNodeId open62541objects.h
     * RAII C++ wrapper class for the UA_ExpandedNodeId struct.
     * Setters are implemented for all member.
     * No getter, use ->member_name to access them.
     * @see UA_ExpandedNodeId in open62541.h
     */
    class ExpandedNodeId : public TypeBase<UA_ExpandedNodeId, UA_TYPES_EXPANDEDNODEID>
    {
    public:
        using TypeBase<UA_ExpandedNodeId, UA_TYPES_EXPANDEDNODEID>::operator=;

        static ExpandedNodeId ModellingRuleMandatory;

        ExpandedNodeId(const std::string namespaceUri, UA_NodeId& node, int serverIndex);

        ExpandedNodeId(const UA_ExpandedNodeId& id);

        /* Returns a non-cryptographic hash for ExpandedNodeId. The hash of an
         * ExpandedNodeId is identical to the hash of the embedded (simple) NodeId if
         * the ServerIndex is zero and no NamespaceUri is set. */
        UA_UInt32 hash() const;
        UA_NodeId& nodeId();
        UA_String& namespaceUri();
        UA_UInt32 serverIndex() const;

        bool toString(std::string& s) const;  // C library version of nodeid to string

        /* Parse the ExpandedNodeId format defined in Part 6, 5.3.1.11:
         *
         *   svr=<serverindex>;ns=<namespaceindex>;<type>=<value>
         *     or
         *   svr=<serverindex>;nsu=<uri>;<type>=<value>
         *
         * The definitions for svr, ns and nsu can be omitted and will be set to zero /
         * the empty string.*/
        bool parse(const std::string& s);

        ExpandedNodeId(const char* chars);

        /** The following functions are shorthand for creating ExpandedNodeIds. */
        ExpandedNodeId(UA_UInt16 nsIndex, UA_UInt32 identifier);
        ExpandedNodeId(UA_UInt16 nsIndex, const std::string& chars);
        ExpandedNodeId(UA_UInt16 nsIndex, char* chars);
        ExpandedNodeId(UA_UInt16 nsIndex, const char* chars);
        ExpandedNodeId(UA_UInt16 nsIndex, UA_Guid guid);
        ExpandedNodeId(UA_UInt16 nsIndex, unsigned char* chars);
        ExpandedNodeId(UA_UInt16 nsIndex, const unsigned char* chars);

        /* Does the ExpandedNodeId point to a local node? That is, are namespaceUri and
         * serverIndex empty? */
        bool isLocal() const;

        /* Total ordering of ExpandedNodeId */
        static UA_Order order(const UA_ExpandedNodeId* n1, const UA_ExpandedNodeId* n2);

        bool operator==(const ExpandedNodeId& e);

        bool operator>(const ExpandedNodeId& e);

        bool operator<(const ExpandedNodeId& e);

    };
} // namespace Open62541


#endif /* EXPANDEDNODEID_H */
