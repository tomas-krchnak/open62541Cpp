
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include "open62541objects.h"
#include <sstream>

namespace Open62541 {

// Standard static nodes
NodeId   NodeId::Objects(0, UA_NS0ID_OBJECTSFOLDER);
NodeId   NodeId::Server(0, UA_NS0ID_SERVER);
NodeId   NodeId::Null(0, 0);
NodeId   NodeId::Organizes(0, UA_NS0ID_ORGANIZES);
NodeId   NodeId::FolderType(0, UA_NS0ID_FOLDERTYPE);
NodeId   NodeId::HasOrderedComponent(0, UA_NS0ID_HASORDEREDCOMPONENT);
NodeId   NodeId::BaseObjectType(0, UA_NS0ID_BASEOBJECTTYPE);
NodeId   NodeId::HasSubType(0, UA_NS0ID_HASSUBTYPE);
NodeId   NodeId::HasModellingRule(0, UA_NS0ID_HASMODELLINGRULE);
NodeId   NodeId::ModellingRuleMandatory(0, UA_NS0ID_MODELLINGRULE_MANDATORY);
NodeId   NodeId::HasComponent(0, UA_NS0ID_HASCOMPONENT);
NodeId   NodeId::HasProperty(0, UA_NS0ID_HASPROPERTY);
NodeId   NodeId::BaseDataVariableType(0, UA_NS0ID_BASEDATAVARIABLETYPE);

ExpandedNodeId   ExpandedNodeId::ModellingRuleMandatory(UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY));


UA_BrowsePathTarget BrowsePathResult::nullResult = { UA_EXPANDEDNODEID_NUMERIC(0, 0), 0 };

void Variant::fromAny(boost::any &a) {
    null(); // clear
    // get the type id as a hash code
    auto t = a.type().hash_code();
    if (t == typeid(std::string).hash_code()) {
        std::string v = boost::any_cast<std::string>(a);
        UA_String ss;
        ss.length = v.size();
        ss.data = (UA_Byte *)(v.c_str());
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }
    else if (t == typeid(int).hash_code()) {
        int v = boost::any_cast<int>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_INT32]);

    }
    else if (t == typeid(char).hash_code()) {
        short v = short(boost::any_cast<char>(a));
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_INT16]);
    }
    else if (t == typeid(bool).hash_code()) {
        bool v = boost::any_cast<bool>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }
    else if (t == typeid(double).hash_code()) {
        double v = boost::any_cast<double>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_DOUBLE]);
    }
    else if (t == typeid(unsigned).hash_code()) {
        unsigned v = boost::any_cast<unsigned>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_UINT32]);
    }
    else if (t == typeid(long long).hash_code()) {
        long long v = boost::any_cast<long long>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_INT64]);
    }
    else if (t == typeid(unsigned long long).hash_code()) {
        unsigned long long v = boost::any_cast<unsigned long long>(a);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_UINT64]);
    }
}

std::string variantToString(UA_Variant &v) {
    std::string ret;
    switch (v.type->typeIndex) {
    case UA_TYPES_BOOLEAN: { // Boolean
            ret = ((UA_Boolean *)(v.data)) ? "true" : "false";
        }
        break;
    case UA_TYPES_SBYTE: { // SByte
            int i = *((char *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_BYTE: { // Byte
            unsigned i = *((unsigned char *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_INT16: { // Int16
            int16_t i = *((int16_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_UINT16: { // UInt16
            uint16_t i = *((uint16_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_INT32: { // Int32
            int32_t i = *((int32_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_UINT32: { // UInt32
            uint32_t i = *((uint32_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_INT64: { // Int64
            int64_t i = *((int64_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_UINT64: { // UInt64
            uint32_t i = *((uint32_t *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_FLOAT: { // Float
            float i = *((float *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_DOUBLE: { // Double
            double i = *((double *)v.data);
            ret = std::to_string(i);
        }
        break;
    case UA_TYPES_STRING: { // String
            UA_String *p = (UA_String *)(v.data);
            ret = std::string((const char *)p->data, p->length);
        }
        break;
    case UA_TYPES_DATETIME: { // DateTime
            UA_DateTime *p = (UA_DateTime *)(v.data);
            UA_DateTimeStruct dts = UA_DateTime_toStruct(*p);
            char b[64];
            int l = sprintf(b, "%02u-%02u-%04u %02u:%02u:%02u.%03u, ",
                   dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec, dts.milliSec);
            ret = std::string(b,l);
        }
        break;
    case UA_TYPES_BYTESTRING: { // ByteString
            UA_ByteString *p = (UA_ByteString *)(v.data);
            ret = std::string((const char *)p->data, p->length);
        }
        break;
    default:
            break;
    }
    return ret;
}

std::string Variant::toString() {
    return variantToString(*(ref()));
}

std::string toString(const UA_NodeId &n) {
    std::string ret = std::to_string(n.namespaceIndex) + ":";

    switch (n.identifierType) {
    case UA_NODEIDTYPE_NUMERIC:
        return ret + std::to_string(n.identifier.numeric);

    case UA_NODEIDTYPE_BYTESTRING:
    case UA_NODEIDTYPE_STRING:
        return ret + std::string((const char *)(n.identifier.string.data), n.identifier.string.length);
    case UA_NODEIDTYPE_GUID: {
        char b[45];
        int l = sprintf(b, "%08X:%04X:%04X[%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X]",
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

        return ret + std::string(b, l);
    }
    default:
        break;
    }
    return std::string("Invalid Node Type");
}

void UANodeTree::printNode(UANode *n, std::ostream &os, int level) {
    if (n) {
        std::string indent(level, ' ');
        os << indent << n->name();
        os << toString(n->data());
        os << std::endl;
        if (n->children().size() > 0) {
            level++;
            for (auto i = n->children().begin(); i != n->children().end(); i++) {
                printNode(i->second, os, level); // recurse
            }
        }
    }
}

UA_StatusCode BrowserBase::browseIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *handle) {
    // node iterator for browsing
    if (isInverse) return UA_STATUSCODE_GOOD; // TO DO what does this do?
    BrowserBase *p = (BrowserBase *)handle;
    if (p) {
        p->process(childId, referenceTypeId); // process record
    }
    return UA_STATUSCODE_GOOD;
}

void BrowserBase::print(std::ostream &os) {
    for (BrowseItem &i : _list) {
        std::string s;
        int j;
        NodeId n;
        n = i.childId;
        if (browseName(n, s, j)) {
            os << toString(i.childId) << " ns:" << i.nameSpace
               << ": "  << i.name  << " Ref:"
               << toString(i.referenceTypeId) << std::endl;
        }
    }
}

BrowseList::iterator BrowserBase::find(const std::string &s) {
    BrowseList::iterator i = _list.begin();
    for (i = _list.begin(); i != _list.end(); i++) {
        BrowseItem &b = *i;
        if (b.name == s) break;
    }
    return i;
}

void BrowserBase::process(UA_NodeId childId,  UA_NodeId referenceTypeId) {
    std::string s;
    int i;
    NodeId n;
    n = childId;
    if (browseName(n, s, i)) {
        _list.push_back(BrowseItem(s, i, childId, referenceTypeId));
    }
}

std::string  timestampToString(UA_DateTime date) {
    UA_DateTimeStruct dts = UA_DateTime_toStruct(date);
    char b[64];
    int l = sprintf(b, "%02u-%02u-%04u %02u:%02u:%02u.%03u, ",
           dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec, dts.milliSec);
    return  std::string(b,l);
}

std::string dataValueToString(UA_DataValue *value) {
    std::stringstream os;
    os << "ServerTime:" <<  timestampToString(value->serverTimestamp) << " ";
    os << "SourceTime:" <<  timestampToString(value->sourceTimestamp) << " ";
    os << "Status:" << std::hex <<  value->status << " ";
    os << "Value:" << variantToString(value->value);
    return os.str();
}

} // namespace Open62541
