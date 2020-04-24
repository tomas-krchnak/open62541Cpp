/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#ifndef OPEN62541OBJECTS_H
#define OPEN62541OBJECTS_H

#if defined(_MSC_VER)
// get rid of template not exported warnings - warning is meaningless as it cannot be fixed
#pragma warning(disable:4251)
#endif
//
#include "open62541.h"
#include "open62541cpp_trace.h"
//
#include <string>
#if defined(__GNUC__)
#include <error.h>
#endif
//
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>
#include "propertytree.h"
#include <boost/any.hpp>
//
// Open 62541 has quasi new-delete and copy operators for each object type
// define wrappers for Open 62541 objects
//
// With Microsoft Windows watch out for class export problems
// Sometimes templates have to include UA_EXPORT other times not
// If the template is typedef'ed do not export
// If the template is the base of a class it is exported
//
namespace Open62541 {

/**
 *  Base wrapper for most C open62541 object types
 *  use unique_ptr
 */ 
template<typename T>
class UA_EXPORT TypeBase {
protected:
    std::unique_ptr<T> _d; // shared pointer - there is no copy on change

public:
    TypeBase(T* t) : _d(t) {}
    T&        get()       const { return *(_d.get()); }
    // Reference and pointer for parameter passing
    operator  T &()       const { return get(); }
    operator  T *()       const { return _d.get(); }
    const T*  constRef()  const { return _d.get(); }
    T*        ref()       const { return _d.get(); }
};
//
// Repeated for each type but cannot use C++ templates because we must also wrap the C function calls for each type
// initialisation implies shallow copy and so takes ownership, assignment is deep copy so source is not owned
//
// trace the object create and delete - only for detailed testing / debugging
#define UA_STRINGIFY(a) __UA_STRINGIFY(a)
#define __UA_STRINGIFY(a) #a

#ifdef UA_TRACE_OBJ
    #define UA_TRC(s) std::cout << s << std::endl;
#else
    #define UA_TRC(s)
#endif
//
// copies are all deep copies
//
#define UA_TYPE_BASE(C,T)\
    C() \
    : TypeBase(T##_new()) { T##_init(_d.get()); UA_TRC("Construct:" << UA_STRINGIFY(C)) } \
    C(const T &t) \
    : TypeBase(T##_new()) { assignFrom(t);      UA_TRC("Construct (" << UA_STRINGIFY(T) << ")") } \
    ~C()                  { UA_TRC("Delete:" << UA_STRINGIFY(C)); if(_d) T##_deleteMembers(_d.get()); } \
    C(const C & n) \
    : TypeBase(T##_new()) { T##_copy(n._d.get(), _d.get()); UA_TRC("Copy Construct:" << UA_STRINGIFY(C)) } \
    C& operator = (const C& n) { UA_TRC("Assign:" << UA_STRINGIFY(C)); null(); T##_copy(n._d.get(), _d.get()); return *this; } \
    void null()           { if(_d) { UA_TRC("Delete(in null):" << UA_STRINGIFY(C)); T##_deleteMembers(_d.get()); } _d.reset(T##_new()); T##_init(_d.get()); } \
    void assignTo(T &v)   { T##_copy(_d.get(), &v); } \
    void assignFrom(const T &v){ T##_copy(&v, _d.get()); }

#define UA_TYPE_DEF(T) UA_TYPE_BASE(T,UA_##T)


/**
 * The Array class
 * This is for allocated arrays of UA_ objects simple lifecycle management.
 * Uses UA_array_new and UA_array_delete rather than new and delete
 * Also deals with arrays returned from UA_xxx functions */
template <typename T, const int I>
class Array {
    size_t  _length = 0;
    T*      _data   = nullptr;

public:
    Array()                         {}
    Array(T* data, size_t len)
        : _data(data), _length(len) {}

    Array(size_t n) { allocate(n); }
    ~Array()        { clear(); }

    void allocate(size_t len) {
        clear();
        _data = (T*)UA_Array_new(len, &UA_TYPES[I]);
        _length = len;
    }

    /**
     * detach and transfer ownership to the caller - no longer managed
     */
    void release() { _length = 0; _data = nullptr; }

    void clear() {
        if (_length && _data) {
            UA_Array_delete(_data, _length, &UA_TYPES[I]);
        }
        _length = 0;
        _data = nullptr;
    }

    T &at(size_t i) const {
        if (!_data || (i >= _length)) throw std::exception();
        return _data[i];
    }

    void setList(size_t len, T *data) {
        clear();
        _length = len;
        _data = data;
    }

    // Accessors
    size_t  length()  const { return _length; }
    T*      data()    const { return _data; }
    size_t* lengthRef()     { return &_length; }
    T**     dataRef()       { return &_data; }
    operator T*()           { return _data; }
}; // class Array

// typedef basic array types
typedef Array<UA_String, UA_TYPES_STRING> StringArray;
typedef Array<UA_NodeId, UA_TYPES_NODEID> NodeIdArray;
typedef Array<UA_Variant, UA_TYPES_VARIANT> VariantArray;
typedef Array<UA_QualifiedName, UA_TYPES_QUALIFIEDNAME> QualifiedNameArray;
typedef Array<UA_SimpleAttributeOperand, UA_TYPES_SIMPLEATTRIBUTEOPERAND> SimpleAttributeOperandArray;
typedef Array<UA_EndpointDescription, UA_TYPES_ENDPOINTDESCRIPTION> EndpointDescriptionArray;
typedef Array<UA_ApplicationDescription, UA_TYPES_APPLICATIONDESCRIPTION> ApplicationDescriptionArray;
typedef Array<UA_ServerOnNetwork, UA_TYPES_SERVERONNETWORK> ServerOnNetworkArray;

// non-heap allocation - no delete
// std::string      -> UA_String
inline UA_String toUA_String(const std::string &s) {
    UA_String r;
    r.length = s.size();
    r.data = (UA_Byte *)(s.c_str());
    return r;
}

// std::string   -> UA_String
inline void fromStdString(const std::string &in, UA_String &out) {
    UA_String_deleteMembers(&out);
    out = UA_STRING_ALLOC(in.c_str());
}

// UA_ByteString -> std::string
inline std::string fromByteString(UA_ByteString &b) { return std::string((const char*)b.data,b.length); }

// UA_String     -> std::string
inline std::string toString(UA_String &r) { return std::string((const char*)(r.data), r.length); }

// UA_Variant    -> std::string
std::string variantToString(UA_Variant &v);

// UA_StatusCode -> std::string
inline std::string toString(UA_StatusCode c) { return std::string(UA_StatusCode_name(c)); }

// UA_DateTime   -> std::string
std::string  timestampToString(UA_DateTime date);

// UA_DataValue  -> std::string
std::string  dataValueToString(UA_DataValue *value);

// UA_NodeId     -> std::string
UA_EXPORT std::string toString(const UA_NodeId &n);

inline void printLastError(UA_StatusCode c, std::iostream &os) {
    os << UA_StatusCode_name(c) ;
}

// Prints status only if not Good
#define UAPRINTLASTERROR(c) {if(c != UA_STATUSCODE_GOOD) std::cerr << __FUNCTION__ << ":" << __LINE__ << ":" << UA_StatusCode_name(c) << std::endl;}

/**
 * Default access control. The log-in can be anonymous or username-password.
 * A logged-in user has all access rights.
 * @class UsernamePasswordLogin open62541objects.h
 * RAII C++ wrapper class for the UA_UsernamePasswordLogin struct
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * public members are username and password UA_String
 */
class UA_EXPORT UsernamePasswordLogin : public TypeBase<UA_UsernamePasswordLogin> {
public:
    UsernamePasswordLogin(const std::string &u = "", const std::string &p = "")
      : TypeBase(new UA_UsernamePasswordLogin()) {
        UA_String_init(&ref()->username);
        UA_String_init(&ref()->password);
        setUserName(u);
        setPassword(p);
    }

    ~UsernamePasswordLogin() {
        UA_String_deleteMembers(&ref()->username);
        UA_String_deleteMembers(&ref()->password);
    }

    void setUserName(const std::string &s) {
        fromStdString(s, ref()->username);
    }

    void setPassword(const std::string &s) {
        fromStdString(s, ref()->password);
    }
};

/**
 * The attributes for an object node. ID: 156
 * @class ObjectAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ObjectAttributes struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @see UA_ObjectAttributes in open62541.h
 */
class UA_EXPORT ObjectAttributes : public TypeBase<UA_ObjectAttributes> {
public:
    UA_TYPE_DEF(ObjectAttributes)
    void setDefault() {
        *this = UA_ObjectAttributes_default;
    }
    void setDisplayName(const std::string &s) {
        get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setSpecifiedAttributes(UA_UInt32 m) {
        get().specifiedAttributes = m;
    }
    void setWriteMask(UA_UInt32 m) {
        get().writeMask = m;
    }
    void setUserWriteMask(UA_UInt32 m) {
        get().userWriteMask = m;
    }
    void setEventNotifier(unsigned m) {
        get().eventNotifier = (UA_Byte)m;
    }
};

/**
 * The attributes for an object type node. ID: 66
 * @class ObjectTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ObjectTypeAttributes struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @see UA_ObjectTypeAttributes in open62541.h
 */
class UA_EXPORT ObjectTypeAttributes : public TypeBase<UA_ObjectTypeAttributes> {
public:
    UA_TYPE_DEF(ObjectTypeAttributes)
    void setDefault() {
        *this = UA_ObjectTypeAttributes_default;
    }

    void setDisplayName(const std::string &s) {
        get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setSpecifiedAttributes(UA_UInt32 m) {
        get().specifiedAttributes = m;
    }
    void setWriteMask(UA_UInt32 m) {
        get().writeMask = m;
    }
    void setUserWriteMask(UA_UInt32 m) {
        get().userWriteMask = m;
    }
    void setIsAbstract(bool f) {
        get().isAbstract = f;
    }
};

/**
 * A mask specifying the class of the node.
 * Alias for UA_NodeClass
 * @see UA_NodeClass in open62541.h
 */
typedef UA_NodeClass NodeClass;

/**
 * An identifier for a node in the address space of an OPC UA Server.
 * @class NodeId open62541objects.h
 * RAII C++ wrapper class for the UA_NodeId struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @see UA_NodeId in open62541.h
 */
class UA_EXPORT NodeId : public TypeBase<UA_NodeId> {
public:

    // Common constant nodes
    static NodeId  Null;
    static NodeId  Objects;
    static NodeId  Server;
    static NodeId  Organizes;
    static NodeId  FolderType;
    static NodeId  HasOrderedComponent;
    static NodeId  BaseObjectType;
    static NodeId  HasSubType;
    static NodeId  HasModellingRule;
    static NodeId  ModellingRuleMandatory;
    static NodeId  HasComponent;
    static NodeId  BaseDataVariableType;
    static NodeId  HasProperty;
    static NodeId  BaseEventType;

    UA_TYPE_DEF(NodeId)

    bool isNull() {
        return UA_NodeId_isNull(ref());
    }

    // equality
    bool operator == (const NodeId &n) {
        return UA_NodeId_equal(_d.get(), n._d.get());
    }
    /**
     * @return a non-cryptographic hash for the NodeId
     */
    unsigned hash() {
        return UA_NodeId_hash(ref());
    }

    // Specialised constructors
    NodeId(unsigned index, unsigned id) : TypeBase(UA_NodeId_new()) {
        *(_d.get()) = UA_NODEID_NUMERIC(UA_UInt16(index), id);
    }

    NodeId(unsigned index, const std::string &id) : TypeBase(UA_NodeId_new()) {
        *(_d.get()) = UA_NODEID_STRING_ALLOC(UA_UInt16(index), id.c_str());
    }

    NodeId(unsigned index, UA_Guid guid) : TypeBase(UA_NodeId_new()) {
        *(_d.get()) = UA_NODEID_GUID(UA_UInt16(index), guid);
    }

    // accessors
    int nameSpaceIndex() {
        return ref()->namespaceIndex;
    }

    UA_NodeIdType identifierType() {
        return ref()->identifierType;
    }

    /**
     * Makes a node not null so new nodes are returned to references.
     * Clear everything in the node before initializing it
     * as a numeric variable node in namespace 1
     * @return a reference to the node.
     */
    NodeId &notNull() {
        null(); // clear anything beforehand
        *(_d.get()) = UA_NODEID_NUMERIC(1, 0); // force a node not to be null
        return *this;
    }
};

/**
 * @class UANodeIdList open62541objects.h
 * RAII vector of UA_NodeId with the put method added.
 * @see UA_NodeId in open62541.h
 */
class UA_EXPORT UANodeIdList : public std::vector<UA_NodeId> {
public:
    UANodeIdList() {}
    virtual ~UANodeIdList() {
        for (int i = 0 ; i < int(size()); i++) {
            UA_NodeId_deleteMembers(&(at(i))); // delete members
        }
    }

    void put(UA_NodeId &n) {
        UA_NodeId i; // deep copy
        UA_NodeId_init(&i);
        UA_NodeId_copy(&n, &i);
        push_back(i);
    }
};

/**
 * @class NodeIdMap open62541objects.h
 * RAII map of name, UA_NodeId with the put method added.
 * @see UA_NodeId in open62541.h
 */
class UA_EXPORT NodeIdMap : public std::map<std::string, UA_NodeId> {
public:
    NodeIdMap() {} // set of nodes not in a tree

    virtual ~NodeIdMap() {
        // delete node data
        for (auto i = begin(); i != end(); i++) {
            UA_NodeId &n = i->second;
            UA_NodeId_deleteMembers(&n);
        }
        clear();
    }

    void put(UA_NodeId &n) {
        UA_NodeId i; // deep copy
        UA_NodeId_init(&i);
        UA_NodeId_copy(&n, &i);
        const std::string s = toString(i);
        insert(std::pair<std::string, UA_NodeId>(s, i));
    }
};

/**
 * A NodeId that allows the namespace URI to be specified instead of an index.
 * @class ExpandedNodeId open62541objects.h
 * RAII C++ wrapper class for the UA_ExpandedNodeId struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @see UA_ExpandedNodeId in open62541.h
 */
class UA_EXPORT ExpandedNodeId : public TypeBase<UA_ExpandedNodeId> {
public:
    UA_TYPE_DEF(ExpandedNodeId)
    static ExpandedNodeId  ModellingRuleMandatory;

    ExpandedNodeId(
      const std::string namespaceUri,
      UA_NodeId &node,
      int serverIndex)
      : TypeBase(UA_ExpandedNodeId_new()) {
        ref()->namespaceUri = UA_STRING_ALLOC(namespaceUri.c_str());
        UA_NodeId_copy(&get().nodeId, &node); // deep copy across
        ref()->serverIndex = serverIndex;
    }

    UA_NodeId& nodeId ()                    { return ref()->nodeId;}
    UA_String& namespaceUri()               { return ref()->namespaceUri;}
    UA_UInt32  serverIndex()                { return ref()->serverIndex;}
};

/**
 * The result of a translate operation. ID: 184
 * @class BrowsePathResult open62541objects.h
 * RAII C++ wrapper class for the UA_BrowsePathResult struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @see UA_BrowsePathResult in open62541.h
 */
class UA_EXPORT BrowsePathResult : public TypeBase<UA_BrowsePathResult> {
    static UA_BrowsePathTarget nullResult;

public:
    UA_TYPE_DEF(BrowsePathResult)
    UA_StatusCode statusCode()        const { return ref()->statusCode; }
    size_t targetsSize()              const { return ref()->targetsSize; }
    UA_BrowsePathTarget targets(size_t i)   { return (i < ref()->targetsSize) ? ref()->targets[i] : nullResult; }
};

/**
 * Variants may contain values of any type together with a description of the content.
 * @class BrowsePathResult open62541objects.h
 * RAII C++ wrapper class for the UA_Variant struct.
 * Setters are implemented for all member.
 * No getter, use get().member_name to access them.
 * @warning potential memory leak
 * @todo check memory leak
 * @see UA_Variant in open62541.h
 */
class UA_EXPORT Variant : public TypeBase<UA_Variant> {
public:
    // It would be nice to template but ...
    UA_TYPE_DEF(Variant)

    // Construct Variant from ...
    // TO DO add array handling
    Variant(const std::string &v) : TypeBase(UA_Variant_new()) {
        UA_String ss;
        ss.length = v.size();
        ss.data = (UA_Byte *)(v.c_str());
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }

    Variant(UA_UInt64 v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_UINT64]);
    }

    Variant(UA_String &v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &v, &UA_TYPES[UA_TYPES_STRING]);
    }

    Variant(const char *v) : TypeBase(UA_Variant_new()) {
        UA_String ss = UA_STRING((char *)v);
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }

    Variant(int a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &a, &UA_TYPES[UA_TYPES_INT32]);
    }

    Variant(unsigned a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &a, &UA_TYPES[UA_TYPES_UINT32]);
    }

    Variant(double a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &a, &UA_TYPES[UA_TYPES_DOUBLE]);
    }

    Variant(bool a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &a, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    Variant(UA_DateTime t) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant *)ref(), &t, &UA_TYPES[UA_TYPES_DATETIME]);
    }

    /**
     * cast to a type supported by UA
     */
    template<typename T> T value() {
        if (!UA_Variant_isEmpty((UA_Variant *)ref())) {
            return *((T *)ref()->data); // cast to a value - to do Type checking needed
        }
        return T();
    }

    bool empty() {
        return UA_Variant_isEmpty(ref());
    }

    void clear() {
        if (!empty() && get().storageType == UA_VARIANT_DATA) {
            UA_Variant_deleteMembers((UA_Variant*)ref());
        }
    }

    /**
     * convert a boost::any to a Variant
     * This is limited to basic types: std::string, bool, char, int, long long, uint, ulong long
     * @param a boost::any
     */
    void fromAny(boost::any &a);

    /**
     * toString
     * @return variant in string form
     * @param n
     * @return Node in string form
     */
    std::string toString();
};

/**
 * A name qualified by a namespace.
 * @class QualifiedName open62541objects.h
 * RAII C++ wrapper class for the UA_QualifiedName struct.
 * Setters are implemented for all member.
 * @see UA_QualifiedName in open62541.h
 */
class UA_EXPORT QualifiedName : public TypeBase<UA_QualifiedName> {
public:
    UA_TYPE_DEF(QualifiedName)
    QualifiedName(int ns, const char *s) : TypeBase(UA_QualifiedName_new()) {
        *(_d.get()) = UA_QUALIFIEDNAME_ALLOC(ns, s);
    }

    QualifiedName(int ns, const std::string &s) : TypeBase(UA_QualifiedName_new()) {
        *(_d.get()) = UA_QUALIFIEDNAME_ALLOC(ns, s.c_str());
    }

    UA_UInt16   namespaceIndex() { return ref()->namespaceIndex;}
    UA_String&  name()           { return ref()->name;}
};

typedef std::vector<std::string> Path;

// BrowseItem must be declare here.
/**
 * @struct BrowseItem open62541objects.h
 * Helper struct for BrowserBase encapsulating the information linked to a node.
 * This information are the name, namespace, the node itself and its type (held in another node)
 * A specialized Ctor initializing all members and a CopyTor are provided.
 * All members are public.
 */
struct UA_EXPORT BrowseItem {
    std::string name;               /**< the node browse name */
    int         nameSpace = 0;      /**< the node namespace index */
    UA_NodeId   childId;            /**< the node id. Should be renamed nodeId */
    UA_NodeId   referenceTypeId;    /**< the node's node type */

    BrowseItem(
        const std::string& n,
        int                ns,
        UA_NodeId          c,
        UA_NodeId          r)
        : name(n)
        , nameSpace(ns)
        , childId(c)
        , referenceTypeId(r) {}

    BrowseItem(const BrowseItem& b)
        : name(b.name)
        , nameSpace(b.nameSpace) {
        childId         = b.childId;
        referenceTypeId = b.referenceTypeId;
    }
};

// Helper containers
class UA_EXPORT ArgumentList : public std::vector<UA_Argument> {
public:
    // use constant strings for argument names - else memory leak
    void addScalarArgument(const char *s, int type) {
        UA_Argument a;
        UA_Argument_init(&a);
        a.description = UA_LOCALIZEDTEXT((char *)"en_US", (char *)s);
        a.name = UA_STRING((char *)s);
        a.dataType = UA_TYPES[type].typeId;
        a.valueRank = -1; /* scalar */
        push_back(a);
    }
    // TODO add array argument types
};

typedef std::vector<UA_Variant> VariantList; // shallow copied

// Wrap method call return value sets
// this takes over management of the returned data
class UA_EXPORT VariantCallResult {
    UA_Variant *_data = nullptr;
    size_t _size = 0;

public:
    VariantCallResult(UA_Variant *d = nullptr, size_t n = 0) : _data(d), _size(n) {}
    ~VariantCallResult() {
        clear();
    }

    void clear() {
        if (_data) {
            UA_Array_delete(_data, _size, &UA_TYPES[UA_TYPES_VARIANT]);
        }
        _data = nullptr;
        _size = 0;
    }

    void set(UA_Variant *d, size_t n) {
        clear();
        _data = d;
        _size = n;
    }

    size_t size() const {
        return _size;
    }

    UA_Variant *data() const {
        return  _data;
    }
};

/**
 * The attributes for a variable node. ID: 27
 * @class VariableAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_VariableAttributes struct.
 * Setters are implemented for all members,
 * except specifiedAttributes, writeMask, userWriteMask, dataType,
 * accessLevel, userAccessLevel, minimumSamplingInterval
 * arrayDimensionsSize and arrayDimensions.
 * No getter, use get().member_name to access them.
 * @todo implement all setters
 * @see UA_VariableAttributes in open62541.h
 */
class UA_EXPORT VariableAttributes : public TypeBase<UA_VariableAttributes> {
public:
    UA_TYPE_DEF(VariableAttributes)
    void setDefault() {
        *this = UA_VariableAttributes_default;
    }
    void setDisplayName(const std::string &s) {
        get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setValue(Variant &v) {
        UA_Variant_copy(v,  &get().value); // deep copy the variant - do not know life times
    }
    void setValueRank(int i) {
        get().valueRank = i;
    }
    void setHistorizing(bool f = true) {
        get().historizing = f;
        if(f) {
            get().accessLevel |=  UA_ACCESSLEVELMASK_HISTORYREAD;
        }
        else {
            get().accessLevel &= ~UA_ACCESSLEVELMASK_HISTORYREAD;
        }
    }
};

/**
 * The attributes for a variable type node. ID: 47
 * @class VariableTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_VariableTypeAttributes struct.
 * Setters are implemented for 2/11 members only
 * No getter, use get().member_name to access them.
 * @todo implement all setters
 * @see UA_VariableTypeAttributes in open62541.h
 */
class UA_EXPORT VariableTypeAttributes : public TypeBase<UA_VariableTypeAttributes> {
public:
    UA_TYPE_DEF(VariableTypeAttributes)
    void setDefault() {
        *this = UA_VariableTypeAttributes_default;
    }
    void setDisplayName(const std::string &s) {
        get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
};

/**
 * The attributes for a method node. ID: 143
 * @class MethodAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_MethodAttributes struct.
 * Setters are implemented for 3/7 members only
 * No getter, use get().member_name to access them.
 * @todo implement all setters
 * @see UA_MethodAttributes in open62541.h
 */
class UA_EXPORT MethodAttributes : public TypeBase<UA_MethodAttributes> {
public:
    UA_TYPE_DEF(MethodAttributes)
    void setDefault() {
        *this = UA_MethodAttributes_default;
    }
    void setDisplayName(const std::string &s) {
        get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setExecutable(bool exe = true, bool user = true) {
        get().executable = exe;
        get().userExecutable = user;
    }
};

/**
 * An argument for a method. ID: 132
 * @class ArgumentList open62541objects.h
 * RAII C++ wrapper class for the UA_Argument struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use get().member_name to access them.
 * @see UA_Argument in open62541.h
 */
class UA_EXPORT Argument : public TypeBase<UA_Argument> {
public:
    UA_TYPE_DEF(Argument)
    void setDataType(int i) {
        get().dataType = UA_TYPES[i].typeId;
    }
    void setDescription(const std::string &s) {
        get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str());
    }
    void setName(const std::string &s) {
        get().name = UA_STRING_ALLOC(s.c_str());
    }
    void setValueRank(int i) {
        get().valueRank = i;   /* scalar argument */
    }
};

/**
 * Human readable text with an optional locale identifier.
 * @class LocalizedText open62541objects.h
 * RAII C++ wrapper class for the UA_LocalizedText struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use get().member_name to access them.
 * @see UA_LocalizedText in open62541.h
 */
class UA_EXPORT LocalizedText : public TypeBase<UA_LocalizedText> {
public:
    UA_TYPE_DEF(LocalizedText)
    LocalizedText(const std::string &locale, const std::string &text) : TypeBase(UA_LocalizedText_new()) {
        get() = UA_LOCALIZEDTEXT_ALLOC(locale.c_str(), text.c_str());
    }
    void setLocal(const std::string &s) {
        get().locale = UA_STRING_ALLOC(s.c_str());
    }
    void setText(const std::string &s) {
        get().text = UA_STRING_ALLOC(s.c_str());
    }
};

/**
 * An element in a relative path. ID: 86
 * @class RelativePathElement open62541objects.h
 * RAII C++ wrapper class for the UA_RelativePathElement struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use get().member_name to access them.
 * @see UA_RelativePathElement in open62541.h
 */
class UA_EXPORT RelativePathElement : public TypeBase<UA_RelativePathElement> {
public:
    UA_TYPE_DEF(RelativePathElement)
    RelativePathElement(
        QualifiedName&  item,
        NodeId&         typeId,
        bool            inverse         = false,
        bool            includeSubTypes = false)
        : TypeBase(UA_RelativePathElement_new()) {
        get().referenceTypeId = typeId.get();
        get().isInverse       = includeSubTypes;
        get().includeSubtypes = inverse;
        get().targetName      = item.get(); // shallow copy!!!
    }
};

/**
 * A relative path constructed from reference types and browse names. ID: 104
 * @class RelativePath open62541objects.h
 * RAII C++ wrapper class for the UA_RelativePath struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_RelativePath in open62541.h
 */
class UA_EXPORT RelativePath : public TypeBase<UA_RelativePath> {
public:
    UA_TYPE_DEF(RelativePath)
};

/**
 * A request to translate a path into a node id. ID: 155
 * @class BrowsePath open62541objects.h
 * RAII C++ wrapper class for the UA_BrowsePath struct.
 * No getter or setter, use get().member_name to access them.
 * @see UABrowsePath in open62541.h
 */
class UA_EXPORT BrowsePath : public TypeBase<UA_BrowsePath> {
public:
    UA_TYPE_DEF(BrowsePath)
    BrowsePath(NodeId &start, RelativePath &path) : TypeBase(UA_BrowsePath_new()) {
        UA_RelativePath_copy(path.constRef(), &get().relativePath); // deep copy
        UA_NodeId_copy(start, &get().startingNode);
    }

    BrowsePath(NodeId &start, RelativePathElement &path) : TypeBase(UA_BrowsePath_new()) {
        get().startingNode = start.get();
        get().relativePath.elementsSize = 1;
        get().relativePath.elements = path.ref();
    }
};

/**
 * The result of a browse operation. ID: 174
 * @class BrowseResult open62541objects.h
 * RAII C++ wrapper class for the UA_BrowseResult struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_BrowseResult in open62541.h
 */
class UA_EXPORT BrowseResult : public TypeBase<UA_BrowseResult> {
public:
    UA_TYPE_DEF(BrowseResult)
};

/**
 * A request to call a method. ID: 61
 * Contains the node of the method to call and the argument list + size.
 * @class CallMethodRequest open62541objects.h
 * RAII C++ wrapper class for the UA_CallMethodRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_CallMethodRequest in open62541.h
 */
class UA_EXPORT CallMethodRequest : public TypeBase<UA_CallMethodRequest> {
public:
    UA_TYPE_DEF(CallMethodRequest)
};

/**
 * The result of method call request. ID: 48
 * Contains:
 * - the result status code
 * - the input argument results array
 * - the input argument diagnostic infos array
 * - the output argument array
 * @class CallMethodResult open62541objects.h
 * RAII C++ wrapper class for the UA_CallMethodResult struct.
 * No getter or setter, use get().member_name to access them.
 * @todo: add accessors for the arrays
 * @see UA_CallMethodResult in open62541.h
 */
class UA_EXPORT CallMethodResult  : public TypeBase<UA_CallMethodResult> {
public:
    UA_TYPE_DEF(CallMethodResult)
};

/**
 * The attributes for a view node. ID: 25
 * @class ViewAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ViewAttributes struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_ViewAttributes in open62541.h
 */
class UA_EXPORT ViewAttributes  : public TypeBase<UA_ViewAttributes> {
public:
    UA_TYPE_DEF(ViewAttributes)
    void setDefault() {
        *this = UA_ViewAttributes_default;
    }
};

/**
 * The attributes for a reference type node. ID: 119
 * @class ReferenceTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ReferenceTypeAttributes struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_ReferenceTypeAttributes in open62541.h
 */
class UA_EXPORT ReferenceTypeAttributes : public TypeBase< UA_ReferenceTypeAttributes> {
public:
    UA_TYPE_DEF(ReferenceTypeAttributes)
    void setDefault() {
        *this = UA_ReferenceTypeAttributes_default;
    }
};

/**
 * The attributes for a data type node. ID: 93
 * @class DataTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_DataTypeAttributes struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_DataTypeAttributes in open62541.h
 */
class UA_EXPORT DataTypeAttributes : public TypeBase<UA_DataTypeAttributes> {
public:
    UA_TYPE_DEF(DataTypeAttributes)
    void setDefault() {
        *this = UA_DataTypeAttributes_default;
    }
};

/**
 * Data Source read and write callbacks.
 * @class DataSource open62541objects.h
 * RAII C++ wrapper class for the UA_DataSource struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_DataSource in open62541.h
 */
class UA_EXPORT DataSource : public TypeBase<UA_DataSource> {
public:
    DataSource()  : TypeBase(new UA_DataSource()) {
        get().read = nullptr;
        get().write = nullptr;
    }
};

// Request / Response wrappers for monitored items and events
/**
 * A request to create a subscription. ID: 162
 * @class CreateSubscriptionRequest open62541objects.h
 * RAII C++ wrapper class for the UA_CreateSubscriptionRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_CreateSubscriptionRequest in open62541.h
 */
class UA_EXPORT CreateSubscriptionRequest : public TypeBase<UA_CreateSubscriptionRequest> {
public:
    UA_TYPE_DEF(CreateSubscriptionRequest)
};

/**
 * Response to a Create Subscription request. ID: 58
 * @class CreateSubscriptionResponse open62541objects.h
 * RAII C++ wrapper class for the UA_CreateSubscriptionResponse struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_CreateSubscriptionResponse in open62541.h
 */
class UA_EXPORT CreateSubscriptionResponse : public TypeBase<UA_CreateSubscriptionResponse> {
public:
    UA_TYPE_DEF(CreateSubscriptionResponse)
};

/**
 * The result of a Create Monitored Item request. ID: 30
 * @class MonitoredItemCreateResult open62541objects.h
 * RAII C++ wrapper class for the UA_MonitoredItemCreateResult struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_MonitoredItemCreateResult in open62541.h
 */
class UA_EXPORT MonitoredItemCreateResult : public TypeBase<UA_MonitoredItemCreateResult> {
public:
    UA_TYPE_DEF(MonitoredItemCreateResult)
};

/**
 * Request to create a monitored item. ID: 128
 * @class MonitoredItemCreateRequest open62541objects.h
 * RAII C++ wrapper class for the UA_MonitoredItemCreateRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_MonitoredItemCreateRequest in open62541.h
 * @warning not to be confused with CreateMonitoredItemsRequest
 */
class UA_EXPORT MonitoredItemCreateRequest : public TypeBase<UA_MonitoredItemCreateRequest> {
public:
    UA_TYPE_DEF(MonitoredItemCreateRequest)
};

/**
 * Response to a Set Monitoring Mode request.  ID: 51
 * @class SetMonitoringModeResponse open62541objects.h
 * RAII C++ wrapper class for the UA_SetMonitoringModeResponse struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_SetMonitoringModeResponse in open62541.h
 */
class UA_EXPORT SetMonitoringModeResponse : public TypeBase<UA_SetMonitoringModeResponse> {
public:
    UA_TYPE_DEF(SetMonitoringModeResponse)
};

/**
 * Request to Set the Monitoring Mode. ID: 117
 * @class SetMonitoringModeRequest open62541objects.h
 * RAII C++ wrapper class for the UA_SetMonitoringModeRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_SetMonitoringModeRequest in open62541.h
 */
class UA_EXPORT SetMonitoringModeRequest : public TypeBase<UA_SetMonitoringModeRequest> {
public:
    UA_TYPE_DEF(SetMonitoringModeRequest)
};

/**
 * Response to a Set Triggering request. ID: 148
 * @class SetTriggeringResponse open62541objects.h
 * RAII C++ wrapper class for the UA_SetTriggeringResponse struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_SetTriggeringResponse in open62541.h
 */
class UA_EXPORT SetTriggeringResponse : public TypeBase<UA_SetTriggeringResponse> {
public:
    UA_TYPE_DEF(SetTriggeringResponse)
};

/**
 * Request to Set Triggering. ID: 173
 * @class SetTriggeringRequest open62541objects.h
 * RAII C++ wrapper class for the UA_SetTriggeringRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_SetTriggeringRequest in open62541.h
 */
class UA_EXPORT SetTriggeringRequest : public TypeBase<UA_SetTriggeringRequest> {
public:
    UA_TYPE_DEF(SetTriggeringRequest)
};

#if 0
/**
 * The configuration of a PubSub connection.
 * @class PubSubConnectionConfig open62541objects.h
 * RAII C++ wrapper class for the UA_PubSubConnectionConfig struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_PubSubConnectionConfig in open62541.h
 */
class UA_EXPORT PubSubConnectionConfig : public TypeBase<UA_PubSubConnectionConfig> {
public:
    UA_TYPE_DEF(PubSubConnectionConfig)
};
#endif

/**
 * A thread-safe tree used to have nodes in a browsable / addressable way.
 */
typedef NodePath<std::string> UAPath;
typedef PropertyTree<std::string, NodeId>::PropertyNode UANode;

class UA_EXPORT UANodeTree : public PropertyTree<std::string, NodeId> {
    NodeId _parent; // note parent node

public:
    UANodeTree(NodeId &p): _parent(p) {
        root().setData(p);
    }

    virtual ~UANodeTree() {}

    NodeId &parent() {
        return  _parent;
    }

    // client and server have different methods - TO DO unify client and server - and template
    // only deal with value nodes and folders - for now
    virtual bool addFolderNode(NodeId & parent, const std::string & s, NodeId & newNode)                { return false; }
    virtual bool addValueNode(NodeId & parent, const std::string & s, NodeId & newNode, Variant & v)    { return false; }
    virtual bool getValue(NodeId &, Variant &)                                                          { return false; }
    virtual bool setValue(NodeId &, Variant &)                                                          { return false; }

    /**
     * Create a path of folder nodes.
     * @param p the path to build
     * @param n specify the starting node for the path creation
     * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the path.
     * @return true on success.
     */
    bool createPathFolders(UAPath &p, UANode *n, int level = 0);
    
    /**
     * Create a path of folder nodes ending with a variable node.
     * @param p the path to build
     * @param n specify the starting node for the path creation
     * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the path.
     * @return true on success.
     */
    bool createPath(UAPath &p, UANode *n, Variant &v, int level = 0);

    /**
     * Set the value of a variable node identified by its full path.
     * If the path doesn't exist, build its missing part 
     * and create the variable node with the given value.
     * setValue() must be overriden for this to succeed.
     * @param p the full path of the variable node.
     * @param v specify the new value for that node.
     * @return true on success.
     * @see setValue
     */
    bool setNodeValue(UAPath &p, Variant &v);

    /**
     * Set the value of a variable node identified by its folder path + its name.
     * If the path doesn't exist, build its missing part 
     * and create the variable node with the given name and value.
     * setValue() must be overriden for this to succeed.
     * @param p the folder path of the variable node.
     * @param child the name of the variable node.
     * @param v specify the new value for that node.
     * @return true on success.
     * @see setValue
     */
    bool setNodeValue(UAPath &p, const std::string &child, Variant &v);

    /**
     * Get the value of a variable node identified by its full path, if it exists.
     * getValue() must be overriden for this to succeed.
     * @param p specify the path of the node to retrieve.
     * @param[out] v return the node's value.
     * @return true on success.
     * @see getValue
     */
    bool getNodeValue(UAPath &p, Variant &v);

    /**
     * Get the value of a variable node identified by its path and name, if it exists.
     * getValue() must be overriden for this to succeed.
     * @param p specify the path of the node to retrieve.
     * @param child the name of the variable node.
     * @param[out] v return the node's value.
     * @return true on success.
     * @see getValue
     */
    bool getNodeValue(UAPath &p, const std::string &child, Variant &v);

   /**
    * Write the descendant tree structure of a node to an output stream.
    * The node name and data are written indented according to their degree in the tree hierarchy.
    * nd1
    *   nd11
    *     nd111
    *   nd12
    *     nd121
    *       nd1211
    *       nd1212
    *     nd122
    *     nd123
    *   nd13
    *     nd131
    * @param n specify the starting node
    * @param os the output stream
    * @param level
    */
    void printNode(UANode *n, std::ostream &os = std::cerr, int level = 0);
};

/**
 * Request to create a monitored item. ID: 169
 * @class CreateMonitoredItemsRequest open62541objects.h
 * RAII C++ wrapper class for the UA_CreateMonitoredItemsRequest struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_CreateMonitoredItemsRequest in open62541.h
 * @warning not to be confused with MonitoredItemCreateRequest
 */
class UA_EXPORT CreateMonitoredItemsRequest : public TypeBase<UA_CreateMonitoredItemsRequest> {
public:
    UA_TYPE_DEF(CreateMonitoredItemsRequest)
};

// used for select clauses in event filtering

/**
 * The EventSelectClause class
 */
class UA_EXPORT EventSelectClauseArray : public SimpleAttributeOperandArray {
public:
    EventSelectClauseArray(size_t n) : SimpleAttributeOperandArray(n) {
        for (size_t i = 0; i < n; i++) {
            at(i).attributeId =  UA_ATTRIBUTEID_VALUE;
            at(i).typeDefinitionId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE);
        }
    }

    void setBrowsePath(size_t i, UAPath &path) {
        if (i < length()) {
            // allocate array
            QualifiedNameArray bp(path.size());
            // set from the path
            for (size_t j = 0; j < bp.length(); j++) {
                // populate
                const std::string &s = path[j];
                bp.at(j) = UA_QUALIFIEDNAME_ALLOC(0, s.c_str());
            }

            at(i).browsePath =    bp.data();
            at(i).browsePathSize = bp.length();
            bp.release();
        }
    }

    void setBrowsePath(size_t i, const std::string &s) {
        UAPath path;
        path.toList(s);
        setBrowsePath(i, path);
    }
};

typedef std::vector<UAPath> UAPathArray; /**< Events work with sets of browse paths */

/**
 * The EventFilter class. ID: 205
 * @class EventFilter open62541objects.h
 * RAII C++ wrapper class for the UA_EventFilter struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_EventFilter in open62541.h
 */
class UA_EXPORT EventFilter : public TypeBase<UA_EventFilter> {
public:
    UA_TYPE_DEF(EventFilter)
};

/**
 * The EventFilterSelect class
 */
class UA_EXPORT EventFilterSelect : public EventFilter {
    EventSelectClauseArray _selectClause; // these must have the life time of the monitored event

public:
    EventFilterSelect() = default;
    EventFilterSelect(size_t i) : _selectClause(i) {}

    ~EventFilterSelect() {
        _selectClause.clear();
    }

    EventSelectClauseArray &selectClause() {
        return _selectClause;
    }

    void setBrowsePaths(UAPathArray &a) {
        //UAPath has all the vector stuff and can parse string paths
        if (a.size()) {
            if (a.size() == _selectClause.length()) {
                for (size_t i = 0; i < a.size(); i++) {
                    _selectClause.setBrowsePath(i, a[i]); // setup a set of browse paths
                }
            }
        }
    }
};

/**
 * The RegisteredServer class. ID: 180
 * @class RegisteredServer open62541objects.h
 * RAII C++ wrapper class for the UA_RegisteredServer struct.
 * No getter or setter, use get().member_name to access them.
 * @see UA_RegisteredServer in open62541.h
 */
class UA_EXPORT RegisteredServer : public TypeBase<UA_RegisteredServer> {
    public:
        UA_TYPE_DEF(RegisteredServer)
};

typedef std::unique_ptr<EventFilterSelect> EventFilterRef;

class UA_EXPORT ClientSubscription;
class UA_EXPORT MonitoredItem;
class UA_EXPORT Server;
class UA_EXPORT Client;
class UA_EXPORT SeverRepeatedCallback;

typedef std::list<BrowseItem> BrowseList;

/**
 * The BrowserBase class provide the basic API for browsing list of nodes.
 * Practically an abstract class and should be inherited from to do something.
 */
class UA_EXPORT BrowserBase {
protected:
    BrowseList _list;

   /**
    * A callback used to iterate over nodes.
    * Iterate over all nodes referenced by a parent Node 
    * by calling the callback function for each child node.
    * It must match the signature of UA_NodeIteratorCallback
    * @param childId
    * @param isInverse specify if the iteration must be done in reverse (not supported). Use False to iterate normally down the tree.
    * @param referenceTypeId
    * @param handle
    * @return status
    * @see UA_NodeIteratorCallback
    */
    static UA_StatusCode browseIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *handle);

public:
    BrowserBase() = default;
    virtual ~BrowserBase()                  { _list.clear(); }
    BrowseList &list()                      { return _list; }

   /**
    * Browse from a starting node.
    * Must be overridden to do anything
    * @param start the id of the browsing starting node
    */
    virtual void browse(UA_NodeId start)    {}

    /**
     * Get the name and namespace index of a given node.
     * Should be customized by derived class.
     * @param[in] node specify the nodeId of the node to read
     * @param[out] name the qualified name of the node
     * @param[out] nsIdx the namespace index of the node
     * @return true if the node was found. On failure the output param should be unchanged.
     */
    virtual bool browseName(
        NodeId&      node,
        std::string& name,
        int&         nsIdx)                 { return false; }
    
   /**
    * Write the content of the list to a given output stream.
    * Each BrowseItem is printed as 
    * <nodeId> ns:<nsIdx>: <name> Ref:<refType>\n
    * @param os a reference to the output stream.
    */
    void print(std::ostream &os);

   /**
    * Search the list for a node matching a given name.
    * @param nodeName the browse name of the node to find
    * @return an iterator to found item or list().end()
    */
    BrowseList::iterator find(const std::string& nodeName);

   /**
    * Populate the _list with the found children nodes.
    * If the given node exists, add its name, namespace,
    * node id and the given reference type in the list of BrowseItem.
    * @param childId the node to store in the list if it exist.
    * @param referenceTypeId additional info stored in the added BrowseItem.
    */
    void process(UA_NodeId childId,  UA_NodeId referenceTypeId);
};


/**
 * Template class permitting to customize what is browsed.
 * @param Browsable a class having a browseName() method matching
 * the signature of BrowserBase::browseName().
 * browse() should be overriden to do something.
 * browseName() is customized by the Browsable::browseName().
 */
template <typename Browsable>
class Browser : public BrowserBase {
    Browsable&  _obj;   /**< a browsable */
    BrowseList  _list;  /**< Why mask BrowserBase::_list? Should be removed? */

public:
    Browser(Browsable &c) : _obj(c) {}
    Browsable &obj() { return _obj; }

    /**
    * Get the name and namespace index of a given node.
    * @param[in] node specify the nodeId of the node to read.
    * @param[out] name the qualified name of the node.
    * @param[out] nsIdx the namespace index of the node.
    * @return true if the node was found. On failure the output param should be unchanged.
    */
    bool browseName(
        NodeId&      node,
        std::string& name,
        int&         nsIdx) override { // BrowserBase
        return _obj.browseName(node, name, nsIdx);
    }
};

} // namespace Open62541

#endif // OPEN62541OBJECTS_H
