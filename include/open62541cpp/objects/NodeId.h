/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef NODEID_H
#define NODEID_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include "open62541/types_generated_handling.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {

/**
 * An identifier for a node in the address space of an OPC UA Server.
 * @class NodeId open62541objects.h
 * RAII C++ wrapper class for the UA_NodeId struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_NodeId in open62541.h
 */
class UA_EXPORT NodeId : public TypeBase<UA_NodeId, UA_TYPES_NODEID>
{
public:
    // Common constant nodes
    static NodeId Null;
    static NodeId Objects;
    static NodeId Server;
    static NodeId Organizes;
    static NodeId FolderType;
    static NodeId HasOrderedComponent;
    static NodeId BaseObjectType;
    static NodeId HasSubType;
    static NodeId HasModellingRule;
    static NodeId ModellingRuleMandatory;
    static NodeId HasComponent;
    static NodeId BaseDataVariableType;
    static NodeId HasProperty;
    static NodeId HasNotifier;
    static NodeId BaseEventType;

    bool isNull() const { return UA_NodeId_isNull(constRef()); }

    explicit operator bool() const { return !isNull(); }

    // equality
    bool operator==(const NodeId& node) { return UA_NodeId_equal(constRef(), node.constRef()); }

    /* Returns a non-cryptographic hash for the NodeId */
    unsigned hash() const { return UA_NodeId_hash(constRef()); }

    NodeId()
        : TypeBase()
    {
    }

    // human friendly id string
    NodeId(const char* id)
        : TypeBase(UA_NodeId_new())
    {
        *(ref()) = UA_NODEID(id);  // parses the string to a node id
    }

    NodeId(const UA_NodeId& t)
        : TypeBase(UA_NodeId_new())
    {
        UA_copy(&t, ref(), &UA_TYPES[UA_TYPES_NODEID]);
    }

    // Specialized constructors
    NodeId(unsigned index, unsigned id)
        : TypeBase(UA_NodeId_new())
    {
        *ref() = UA_NODEID_NUMERIC(UA_UInt16(index), id);
    }

    NodeId(unsigned index, const std::string& id)
        : TypeBase(UA_NodeId_new())
    {
        null();
        *ref() = UA_NODEID_STRING_ALLOC(UA_UInt16(index), id.c_str());
    }

    NodeId(unsigned index, UA_Guid guid)
        : TypeBase(UA_NodeId_new())
    {
        *ref() = UA_NODEID_GUID(UA_UInt16(index), guid);
    }

    // accessors
    int nameSpaceIndex() const { return constRef()->namespaceIndex; }

    UA_NodeIdType identifierType() const { return constRef()->identifierType; }

    /**
     * Makes a node not null so new nodes are returned to references.
     * Clear everything in the node before initializing it
     * as a numeric variable node in namespace 1
     * @return a reference to the node.
     */
    NodeId& notNull()
    {
        null();                            // clear anything beforehand
        *ref() = UA_NODEID_NUMERIC(1, 0);  // force a node not to be null
        return *this;
    }

    UA_UInt32 numeric() const { return constRef()->identifier.numeric; }
    const UA_String& string() { return constRef()->identifier.string; }
    const UA_Guid& guid() { return constRef()->identifier.guid; }
    const UA_ByteString& byteString() { return constRef()->identifier.byteString; }

    const UA_DataType* findDataType() const { return UA_findDataType(constRef()); }

    bool toString(std::string& s) const  // C library version of nodeid to string
    {
        UA_String o;
        UA_NodeId_print(this->constRef(), &o);
        s = std::string((char*)o.data, o.length);
        UA_String_clear(&o);
        return true;
    }
};
}  // namespace Open62541


#endif /* NODEID_H */
