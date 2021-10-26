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
#ifdef UA_ENABLE_AMALGAMATION
#include "open62541.h"
#else
#include "open62541/config.h"
#include "open62541/statuscodes.h"
#include "open62541/nodeids.h"
#include "open62541/common.h"
#include "open62541/types.h"
#include "open62541/util.h"
#include "open62541/client.h"
#include "open62541/server.h"
#include "open62541/architecture_definitions.h"
#include "open62541/server_pubsub.h"
#include "open62541/types_generated.h"
#include "open62541/network_tcp.h"
#include "open62541/client_config_default.h"
#include "open62541/client_highlevel_async.h"
#include "open62541/types_generated_handling.h"
#include "open62541/architecture_functions.h"
#include "open62541/posix/ua_architecture.h"
#include "open62541/server_config_default.h"
#include "open62541/client_subscriptions.h"
#include "open62541/client_highlevel.h"
#include "open62541/plugin/historydatabase.h"
#include "open62541/plugin/log_syslog.h"
#include "open62541/plugin/network.h"
#include "open62541/plugin/historydata/history_database_default.h"
#include "open62541/plugin/historydata/history_data_backend.h"
#include "open62541/plugin/historydata/history_data_gathering.h"
#include "open62541/plugin/historydata/history_data_gathering_default.h"
#include "open62541/plugin/historydata/history_data_backend_memory.h"
#include "open62541/plugin/log.h"
#include "open62541/plugin/nodestore.h"
#include "open62541/plugin/pki.h"
#include "open62541/plugin/pubsub.h"
#include "open62541/plugin/nodestore_default.h"
#include "open62541/plugin/log_stdout.h"
#include "open62541/plugin/securitypolicy.h"
#include "open62541/plugin/accesscontrol.h"
#include "open62541/plugin/accesscontrol_default.h"
#include "open62541/plugin/pki_default.h"
#include "open62541/plugin/securitypolicy_default.h"
#endif
//
#if UA_MULTITHREADING >= 100
// Sleep is function call in wxWidgets
#include <pthread.h>
#undef Sleep
#endif
//
#include "open62541cpp_trace.h"
//
#include <string>
#include <stdint.h>
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
 * Base wrapper for most C open62541 object types
 * use unique_ptr for ownership.
 * Derived type CANT'T have member has the -> operator
 * is overloaded to return member of the underlying UA_object.
 */
template<typename T>
class UA_EXPORT TypeBase {
protected:
    std::unique_ptr<T> _d; /**< there is no copy on change. Can't be private due to UA_TYPE_BASE macro */

public:
    TypeBase(T* pUAobject) : _d(pUAobject) {}
    const T&  get()         const { return *(_d.get()); }
    // Reference and pointer for parameter passing
    operator  T&()          const { return *(_d.get()); } /**< c-style cast to UA_obj& */
    operator  T*()          const { return _d.get(); }    /**< c-style cast to UA_obj* */
    const T*  constRef()    const { return _d.get(); }
    T*        ref()               { return _d.get(); }
    const T*  operator->()  const { return constRef(); }
    T*        operator->()        { return ref(); }
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
    C() :  TypeBase(T##_new()) {T##_init(_d.get()); UA_TRC("Construct:" << UA_STRINGIFY(C))}\
    C(const T &t) :  TypeBase(T##_new()){assignFrom(t);UA_TRC("Construct (" << UA_STRINGIFY(T) << ")")}\
    ~C(){UA_TRC("Delete:" << UA_STRINGIFY(C)); if(_d) T##_clear(_d.get());}\
    C(const C & n) :  TypeBase(T##_new())  { T##_copy(n._d.get(),_d.get()); UA_TRC("Copy Construct:" << UA_STRINGIFY(C))}\
    C & operator = ( const C &n) {UA_TRC("Assign:" << UA_STRINGIFY(C));null(); T##_copy(n._d.get(),_d.get()); return *this;}\
    void null() {if(_d){UA_TRC("Delete(in null):" << UA_STRINGIFY(C));T##_clear(_d.get());} _d.reset(T##_new());T##_init(_d.get());}\
    void assignTo(T &v){ T##_copy(_d.get(),&v);}\
    void assignFrom(const T &v){ T##_copy(&v,_d.get());}

/*!
 * \brief The String class
 */
class String {
    UA_String _s;
public:
    String(const std::string &s)
    {
        _s = UA_String_fromChars(s.c_str());
    }

    String(const String &s)
    {
        UA_String_clear(&_s);
        UA_String_copy(&s._s,&_s);
    }

    String(const UA_String &s)
    {
        UA_String_clear(&_s);
        UA_String_copy(&s,&_s);
    }

    ~String()
    {
        UA_String_clear(&_s);
    }

    operator const UA_String & () {
        return _s;
    }
    operator const UA_String * () {
        return &_s;
    }
    operator UA_String * () {
        return &_s;
    }


    String & operator = (const String &s)
    {
        UA_String_clear(&_s);
        UA_String_copy(&s._s,&_s);
        return *this;
    }

    String & operator = (const UA_String &s)
    {
        UA_String_clear(&_s);
        UA_String_copy(&s,&_s);
        return *this;
    }

    std::string toStdString()
    {
        return std::string((char *)(_s.data),_s.length);
    }


};


class ByteString {
    UA_ByteString _s;
public:
    ByteString(const std::string &s)
    {
        _s = UA_BYTESTRING_ALLOC(s.c_str());
    }

    ByteString(const ByteString &s)
    {
        UA_ByteString_clear(&_s);
        UA_ByteString_copy(&s._s,&_s);
    }

    ByteString(const UA_ByteString &s)
    {
        UA_ByteString_clear(&_s);
        UA_ByteString_copy(&s,&_s);
    }

    ~ByteString()
    {
        UA_ByteString_clear(&_s);
    }

    operator const UA_ByteString & () {
        return _s;
    }
    operator const UA_ByteString * () {
        return &_s;
    }
    operator UA_ByteString * () {
        return &_s;
    }


    ByteString & operator = (const ByteString &s)
    {
        UA_ByteString_clear(&_s);
        UA_ByteString_copy(&s._s,&_s);
        return *this;
    }

    ByteString & operator = (const UA_ByteString &s)
    {
        UA_ByteString_clear(&_s);
        UA_ByteString_copy(&s,&_s);
        return *this;
    }

    std::string toStdString()
    {
        return std::string((char *)(_s.data),_s.length);
    }


};



// It would be nice to template but ...
#define UA_TYPE_DEF(T) UA_TYPE_BASE(T,UA_##T)


template <typename T, const UA_UInt32 I>

/**
 * The Array class
 * This is for allocated arrays of UA_xxx objects simple lifecycle management.
 * Uses UA_array_new and UA_array_delete rather than new and delete
 * Also deals with arrays returned from UA_xxx functions.
 * @param T specify the UA object of the items, ie: UA_String.
 * @param I specify the UA type id of the items, ie: UA_TYPES_STRING.
 */
template <typename T, const UA_UInt32 I>
class Array {
    size_t  m_size  = 0;
    T*      m_pData = nullptr;

public:
    Array() { release(); }
    Array(T* data, size_t len)
        : m_pData(data), m_size(len) {}

    Array(size_t size)  { allocate(size); }
    ~Array()            { clear(); }

    auto& allocate(size_t len) {
        clear();
        m_pData = (T*)UA_Array_new(len, &UA_TYPES[I]);
        m_size = len;
        return *this;
    }

    /**
     * detach and transfer ownership to the caller - no longer managed
     */
    auto& release() { m_size = 0; m_pData = nullptr; return *this; }


    virtual void clearFunc(T *){}
    /*!
        \brief clear
    */
    auto& clear() {
        
        if (m_size && m_pData) {
            T* p = m_pData;
            for (size_t i = 0; i < m_size; i++, p++)
            {
                clearFunc(p);
            }
            UA_Array_delete(m_pData, m_size, &UA_TYPES[I]);
        }
        m_size = 0;
        m_pData = nullptr;
        return *this;
    }

    T& at(size_t idx0) const {
        if (!m_pData || (idx0 >= m_size)) throw std::exception();
        return m_pData[idx0];
    }

    auto& setList(size_t len, T* data) {
        clear();
        m_size = len;
        m_pData = data;
        return *this;
    }

    // Accessors
    size_t  size()    const { return m_size; }
    T*      data()    const { return m_pData; }
    size_t* sizeRef()       { return &m_size; }
    T**     dataRef()       { return &m_pData; }
    operator T*()           { return m_pData; }

    virtual void initFunc(T*) {}
    void prepare()
    {
        if (_length > 0)
        {
            T* p = _data;
            for (size_t i = 0; i < _length; i++, p++)
            {
                initFunc(p);
            }
        }
    }

    // Iterator: so Array is usable in range for loop
    class iterator {
        T* ptr;

    public:
        iterator(T* ptr) : ptr(ptr)                   {}
        iterator operator++()                         { ++ptr; return *this; }
        bool     operator!=(const iterator& o)  const { return ptr != o.ptr; }
        const T& operator*()                    const { return *ptr; }
    };

    iterator begin() const { return iterator(m_pData); }
    iterator end()   const { return iterator(m_pData + m_size); }

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
typedef Array<UA_BrowsePathTarget, UA_TYPES_BROWSEPATHTARGET> BrowsePathTargetArray;

//
// typedef basic array types
// macro to define the init and clear functions for the complex objects
#define ARRAY_INIT_CLEAR(T) void clearFunc(T * p) { T##_clear(p);} void initFunc(T * p) { T##_init(p);}

typedef std::vector<std::string> StdStringArray;

// Generate an array declaration with the type specific constructors and destructors - has to the use the UA versions
#define TYPEDEF_ARRAY(T,I) \
typedef Array<UA_##T, I> T##Array_Base;\
class T##Array : public  T##Array_Base\
{\
public:\
    T##Array() {}\
    T##Array(UA_##T *data, size_t len):T##Array_Base(data,len){}\
    T##Array(size_t n):T##Array_Base(n) {}\
    ARRAY_INIT_CLEAR(UA_##T)\
};
TYPEDEF_ARRAY(String, UA_TYPES_STRING)
TYPEDEF_ARRAY(NodeId, UA_TYPES_NODEID)

// non-heap allocation - no delete
// std::string      -> UA_String
UA_String toUA_String(const std::string& str);

// std::string   -> UA_String
void fromStdString(const std::string& in, UA_String& out);

// UA_ByteString -> std::string
inline std::string fromByteString(const UA_ByteString& uaByte) 
{ 
    return std::string((const char*)uaByte.data, uaByte.length); 
}

// UA_String     -> std::string
inline std::string toString(const UA_String& str) 
{ 
    return std::string((const char*)(str.data), str.length); 
}

// UA_Variant    -> std::string
std::string variantToString(const UA_Variant& variant);

// UA_StatusCode -> std::string
inline std::string toString(UA_StatusCode code) { return std::string(UA_StatusCode_name(code)); }

// UA_DateTime   -> std::string
std::string  timestampToString(UA_DateTime date);

// UA_DataValue  -> std::string
std::string  dataValueToString(const UA_DataValue& value);

// UA_NodeId     -> std::string
UA_EXPORT std::string toString(const UA_NodeId& node);

inline void printLastError(UA_StatusCode code, std::iostream& os) {
    os << UA_StatusCode_name(code) ;
}

// Prints status only if not Good
#define UAPRINTLASTERROR(c) {if(c != UA_STATUSCODE_GOOD) std::cerr << __FUNCTION__ << ":" << __LINE__ << ":" << UA_StatusCode_name(c) << std::endl;}

/**
 * Default access control. The log-in can be anonymous or username-password.
 * A logged-in user has all access rights.
 * @class UsernamePasswordLogin open62541objects.h
 * RAII C++ wrapper class for the UA_UsernamePasswordLogin struct
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * public members are username and password UA_String
 */
class UA_EXPORT UsernamePasswordLogin : public TypeBase<UA_UsernamePasswordLogin> {
public:
    UsernamePasswordLogin(const std::string& u = "", const std::string& p = "")
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

    UsernamePasswordLogin& setUserName(const std::string& str) {
        fromStdString(str, ref()->username);
        return *this;
    }

    UsernamePasswordLogin& setPassword(const std::string& str) {
        fromStdString(str, ref()->password);
        return *this;
    }
};

/**
 * The attributes for an object node. ID: 156
 * @class ObjectAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ObjectAttributes struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_ObjectAttributes in open62541.h
 */
class UA_EXPORT ObjectAttributes : public TypeBase<UA_ObjectAttributes> {
public:
    UA_TYPE_DEF(ObjectAttributes)

    ObjectAttributes(const std::string& name);

    auto& setDefault() {
        *this = UA_ObjectAttributes_default;
        return *this;
    }
    auto& setDisplayName(const std::string& name) {
        ref()->displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.c_str());
        return *this;
    }
    auto& setDescription(const std::string& descr) {
        ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
        return *this;
    }
    auto& setSpecifiedAttributes(UA_UInt32 attribute) {
        ref()->specifiedAttributes = attribute;
        return *this;
    }
    auto& setWriteMask(UA_UInt32 mask) {
        ref()->writeMask = mask;
        return *this;
    }
    auto& setUserWriteMask(UA_UInt32 mask) {
        ref()->userWriteMask = mask;
        return *this;
    }
    auto& setEventNotifier(unsigned event) {
        ref()->eventNotifier = (UA_Byte)event;
        return *this;
    }
};

/**
 * The attributes for an object type node. ID: 66
 * @class ObjectTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ObjectTypeAttributes struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_ObjectTypeAttributes in open62541.h
 */
class UA_EXPORT ObjectTypeAttributes : public TypeBase<UA_ObjectTypeAttributes> {
public:
    UA_TYPE_DEF(ObjectTypeAttributes)
    auto& setDefault() {
        *this = UA_ObjectTypeAttributes_default;
        return *this;
    }

    auto& setDisplayName(const std::string& name) {
        ref()->displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.c_str());
        return *this;
    }
    auto& setDescription(const std::string& descr) {
        ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
        return *this;
    }
    auto& setSpecifiedAttributes(UA_UInt32 attribute) {
        ref()->specifiedAttributes = attribute;
        return *this;
    }
    auto& setWriteMask(UA_UInt32 mask) {
        ref()->writeMask = mask;
        return *this;
    }
    auto& setUserWriteMask(UA_UInt32 mask) {
        ref()->userWriteMask = mask;
        return *this;
    }
    auto& setIsAbstract(bool abstract) {
        ref()->isAbstract = abstract;
        return *this;
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
 * No getter, use ->member_name to access them.
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
    static NodeId  HasNotifier;

    UA_TYPE_DEF(NodeId)

    bool isNull() const {
        return UA_NodeId_isNull(constRef());
    }

    explicit operator bool() const { return !isNull(); }

    // equality
    bool operator == (const NodeId& node) {
        return UA_NodeId_equal(constRef(), node.constRef());
    }
    /**
     * @return a non-cryptographic hash for the NodeId
     */
    unsigned hash() const {
        return UA_NodeId_hash(constRef());
    }

    // Specialized constructors
    NodeId(unsigned index, unsigned id) : TypeBase(UA_NodeId_new()) {
        *ref() = UA_NODEID_NUMERIC(UA_UInt16(index), id);
    }

    NodeId(unsigned index, const std::string& id) : TypeBase(UA_NodeId_new()) {
        *ref() = UA_NODEID_STRING_ALLOC(UA_UInt16(index), id.c_str());
    }

    NodeId(unsigned index, UA_Guid guid) : TypeBase(UA_NodeId_new()) {
        *ref() = UA_NODEID_GUID(UA_UInt16(index), guid);
    }

    // accessors
    int nameSpaceIndex() const {
        return get().namespaceIndex;
    }

    UA_NodeIdType identifierType() const {
        return get().identifierType;
    }

    /**
     * Makes a node not null so new nodes are returned to references.
     * Clear everything in the node before initializing it
     * as a numeric variable node in namespace 1
     * @return a reference to the node.
     */
    NodeId& notNull() {
        null(); // clear anything beforehand
        *ref() = UA_NODEID_NUMERIC(1, 0); // force a node not to be null
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
    virtual ~UANodeIdList();
    void put(const UA_NodeId& node);
};

/**
 * @class NodeIdMap open62541objects.h
 * RAII map of name, UA_NodeId with the put method added.
 * @see UA_NodeId in open62541.h
 */
class UA_EXPORT NodeIdMap : public std::map<std::string, UA_NodeId> {
public:
    NodeIdMap() {} // set of nodes not in a tree
    virtual ~NodeIdMap();
    void put(const UA_NodeId& node);
};

/**
 * A NodeId that allows the namespace URI to be specified instead of an index.
 * @class ExpandedNodeId open62541objects.h
 * RAII C++ wrapper class for the UA_ExpandedNodeId struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_ExpandedNodeId in open62541.h
 */
class UA_EXPORT ExpandedNodeId : public TypeBase<UA_ExpandedNodeId> {
public:
    UA_TYPE_DEF(ExpandedNodeId)
    static ExpandedNodeId  ModellingRuleMandatory;

    ExpandedNodeId(
      const std::string namespaceUri,
      UA_NodeId& outNode,
      int serverIndex);

    UA_NodeId& nodeId ()            { return ref()->nodeId;}
    UA_String& namespaceUri()       { return ref()->namespaceUri;}
    UA_UInt32  serverIndex()  const { return get().serverIndex;}


    bool toString(std::string &s) const // C library version of nodeid to string
    {
        UA_String o;
        UA_ExpandedNodeId_print(this->constRef(), &o);
        s = std::string((char *)o.data,o.length);
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
    bool parse(const std::string &s)
    {
        UA_String str;
        str.data = (UA_Byte *)s.c_str();
        str.length = s.length();
        return UA_ExpandedNodeId_parse(ref(),str) == UA_STATUSCODE_GOOD;
    }

    ExpandedNodeId(const char *chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() =  UA_EXPANDEDNODEID(chars);
    }

    /** The following functions are shorthand for creating ExpandedNodeIds. */
    ExpandedNodeId(UA_UInt16 nsIndex, UA_UInt32 identifier) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_NUMERIC(nsIndex, identifier);
    }

    ExpandedNodeId(UA_UInt16 nsIndex,const std::string &chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars.c_str());
    }


    ExpandedNodeId(UA_UInt16 nsIndex, char *chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_STRING( nsIndex, chars);
    }

    ExpandedNodeId(UA_UInt16 nsIndex,const char *chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_STRING_ALLOC(nsIndex, chars);
    }

    ExpandedNodeId(UA_UInt16 nsIndex, UA_Guid guid) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_STRING_GUID(nsIndex,  guid);
    }

    ExpandedNodeId(UA_UInt16 nsIndex, unsigned char *chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_BYTESTRING(nsIndex, (char *)chars);
    }

    ExpandedNodeId(UA_UInt16 nsIndex,const unsigned char *chars) : TypeBase(UA_ExpandedNodeId_new())
    {
        get() = UA_EXPANDEDNODEID_BYTESTRING_ALLOC(nsIndex, (char *)chars);
    }

    /* Does the ExpandedNodeId point to a local node? That is, are namespaceUri and
     * serverIndex empty? */
    bool isLocal() const
    {
        return UA_ExpandedNodeId_isLocal(constRef()) == UA_TRUE;
    }

    /* Total ordering of ExpandedNodeId */
    static UA_Order order(const UA_ExpandedNodeId *n1, const UA_ExpandedNodeId *n2)
    {
        return UA_ExpandedNodeId_order(n1, n2);
    }
    bool operator == (const ExpandedNodeId &e )
    {
        return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_EQ;
    }

    bool operator > (const ExpandedNodeId &e)
    {
        return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_MORE;
    }

    bool operator < (const ExpandedNodeId &e)
    {
        return UA_ExpandedNodeId_order(constRef(), e.constRef()) == UA_ORDER_LESS;
    }

    /* Returns a non-cryptographic hash for ExpandedNodeId. The hash of an
     * ExpandedNodeId is identical to the hash of the embedded (simple) NodeId if
     * the ServerIndex is zero and no NamespaceUri is set. */
    UA_UInt32 hash() const {
        return  UA_ExpandedNodeId_hash(constRef());
    }
    UA_NodeId & nodeId () {
        return ref()->nodeId;
    }
    UA_String & namespaceUri() {
        return ref()->namespaceUri;
    }
    UA_UInt32 serverIndex() {
        return ref()->serverIndex;
    }
};

/**
 * The result of a translate operation. ID: 184
 * @class BrowsePathResult open62541objects.h
 * RAII C++ wrapper class for the UA_BrowsePathResult struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @see UA_BrowsePathResult in open62541.h
 */
class UA_EXPORT BrowsePathResult : public TypeBase<UA_BrowsePathResult> {
    static UA_BrowsePathTarget nullResult;

public:
    UA_TYPE_DEF(BrowsePathResult)
    UA_StatusCode statusCode()              const { return get().statusCode; }
    size_t targetsSize()                    const { return get().targetsSize; }
    UA_BrowsePathTarget target(size_t idx0) const { return (idx0 < get().targetsSize) ? get().targets[idx0] : nullResult; }
    BrowsePathTargetArray targets()         const { return BrowsePathTargetArray(get().targets, get().targetsSize); }
};

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
/**
 * Variants may contain values of any type together with a description of the content.
 * @class Variant open62541objects.h
 * RAII C++ wrapper class for the UA_Variant struct.
 * Setters are implemented for all member.
 * No getter, use ->member_name to access them.
 * @warning potential memory leak
 * @todo check memory leak
 * @see UA_Variant in open62541.h
 */
class UA_EXPORT Variant : public TypeBase<UA_Variant> {
    /**
    * Configure the variant as a one dimension array.
    * @param size specify the size of the array.
    */
    void set1DArray(size_t size);

public:
    UA_TYPE_DEF(Variant)

    // Scalar Ctor
    template<typename T>
    Variant(const T& val) : TypeBase(UA_Variant_new()) {
      UA_Variant_setScalarCopy(ref(), &val, GetUAPrimitiveType(val));
    }

    // Specialization using overload, not function template full specialization
    Variant(const std::string& str) : TypeBase(UA_Variant_new()) {
        const auto ss = toUA_String(str);
        UA_Variant_setScalarCopy(ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }

    Variant(const char* locale, const char* text) : TypeBase(UA_Variant_new()) {
        UA_LocalizedText t = UA_LOCALIZEDTEXT((char*)locale, (char*)text); // just builds does not allocate
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &t, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    }

    Variant(UA_UInt64 v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &v, &UA_TYPES[UA_TYPES_UINT64]);
    }

    Variant(UA_UInt16 v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &v, &UA_TYPES[UA_TYPES_UINT16]);
    }

    Variant(UA_String& v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &v, &UA_TYPES[UA_TYPES_STRING]);
    }

    Variant(const char* v) : TypeBase(UA_Variant_new()) {
        UA_String ss = UA_STRING((char*)v);
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }

    /*!
        \brief uaVariant
        \param a
    */
    Variant(int a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &a, &UA_TYPES[UA_TYPES_INT32]);
    }

    /*!
        \brief uaVariant
        \param a
    */
    Variant(unsigned a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &a, &UA_TYPES[UA_TYPES_UINT32]);
    }

    /*!
        \brief uaVariant
        \param a
    */
    Variant(double a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &a, &UA_TYPES[UA_TYPES_DOUBLE]);
    }

    /*!
        \brief uaVariant
        \param a
    */
    Variant(bool a) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &a, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }

    /*!
        \brief Variant
        \param t
    */
    Variant(UA_DateTime t) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &t, &UA_TYPES[UA_TYPES_DATETIME]);
    }
    Variant(const char* str) : TypeBase(UA_Variant_new()) {
        const auto ss = UA_STRING((char*)str);
        UA_Variant_setScalarCopy(ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
    }

    // Array Ctor
    template<typename T>
    Variant(const std::vector<T>& vec)
        : TypeBase(UA_Variant_new()) {
      UA_Variant_setArrayCopy(ref(), vec.data(), vec.size(), GetUAPrimitiveType(T()));
      set1DArray(vec.size());
    }

    // Specialization using overload, not function template full specialization
    template<>
    Variant(const std::vector<std::string>& vec)
        : TypeBase(UA_Variant_new()) {
      std::vector<UA_String> ua;
      ua.reserve(vec.size());

      for (const auto& str : vec)
          ua.emplace_back(toUA_String(str));

      UA_Variant_setArrayCopy(ref(), ua.data(), ua.size(), &UA_TYPES[UA_TYPES_STRING]);
      set1DArray(ua.size());
    }

    /**
     * cast to a type supported by UA
     */
    template<typename T>
    T value() {
        if (!empty()) {
            return *((T*)ref()->data); // cast to a value - to do Type checking needed
        }
        return T();
    }

    /**
     * Test if the variant doesn't contain any variable
     * @return true if the the variant is empty.
     */
    bool empty() {
        return UA_Variant_isEmpty(ref());
    }

    /**
     * Clear the variant content, making it empty.
     * @return itself permitting setter chaining.
     */
    Variant& clear();

    /**
     * convert a boost::any to a Variant
     * This is limited to basic types: std::string, bool, char, int, long long, uint, ulong long
     * @param a boost::any
     * @return itself permitting setter chaining.
     */
    Variant& fromAny(const boost::any& anything);

    bool isScalar() const { return   UA_Variant_isScalar(constRef()); }
    bool hasScalarType(const UA_DataType* type) { return   UA_Variant_hasScalarType(constRef(), type); }
    bool hasArrayType(const UA_DataType* type) { return  UA_Variant_hasArrayType(constRef(), type); }
    void setScalar(void* UA_RESTRICT p, const UA_DataType* type) { UA_Variant_setScalar(ref(), p, type); }
    bool  setScalarCopy(const void* p, const UA_DataType* type) { return   UA_Variant_setScalarCopy(ref(), p, type) == UA_STATUSCODE_GOOD; }
    void setArray(void* UA_RESTRICT array, size_t arraySize, const UA_DataType* type) { UA_Variant_setArray(ref(), array, arraySize, type); }
    bool setArrayCopy(const void* array, size_t arraySize, const UA_DataType* type) { return  UA_Variant_setArrayCopy(ref(), array, arraySize, type) == UA_STATUSCODE_GOOD; }
    bool copyRange(Variant& src, const UA_NumericRange range) { return  UA_Variant_copyRange(src.ref(), ref(), range) == UA_STATUSCODE_GOOD; } // copy to this variant
    static bool copyRange(Variant& src, Variant& dst, const UA_NumericRange range) { return  UA_Variant_copyRange(src.ref(), dst.ref(), range) == UA_STATUSCODE_GOOD; }
    bool setRange(void* UA_RESTRICT array, size_t arraySize, const UA_NumericRange range) { return  UA_Variant_setRange(ref(), array, arraySize, range) == UA_STATUSCODE_GOOD; }
    //
    const UA_DataType* dataType() { return ref()->type; }
    bool isType(const UA_DataType* i) { return ref()->type == i; } // compare types
    bool isType(UA_Int32 i) { return ref()->type == &UA_TYPES[i]; } // compare types
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
    QualifiedName(int ns, const char* str) : TypeBase(UA_QualifiedName_new()) {
        *ref() = UA_QUALIFIEDNAME_ALLOC(ns, str);
    }

    QualifiedName(int ns, const std::string& str) : TypeBase(UA_QualifiedName_new()) {
        *ref() = UA_QUALIFIEDNAME_ALLOC(ns, str.c_str());
    }

    UA_UInt16   namespaceIndex() const { return get().namespaceIndex; }
    UA_String&  name()                 { return ref()->name; }
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
    std::string name;           /**< the node browse name */
    int         nameSpace = 0;  /**< the node namespace index */
    UA_NodeId   nodeId;         /**< the node id */
    UA_NodeId   type;           /**< the node's node type */

    BrowseItem(
        const std::string& t_name,
        int                t_nsIdx,
        UA_NodeId          t_node,
        UA_NodeId          t_type)
        : name(t_name)
        , nameSpace(t_nsIdx)
        , nodeId(t_node)
        , type(t_type) {}

    BrowseItem(const BrowseItem& item)
        : name(item.name)
        , nameSpace(item.nameSpace)
        , nodeId(item.nodeId)
        , type(item.type) {}
};

// Helper containers
class UA_EXPORT ArgumentList : public std::vector<UA_Argument> {
public:
    // use constant strings for argument names - else memory leak
    void addScalarArgument(const char* name, int type);
    // TODO add array argument types
};


/*!
    \brief VariantList
*/
typedef std::vector<UA_Variant> VariantList; // shallow copied

// Wrap method call return value sets
/*!
    \brief The VariantCallResult class
*/
// this takes over management of the returned data
class  UA_EXPORT  VariantCallResult {
    UA_Variant *_data = nullptr;
    size_t _size = 0;
public:
    /*!
        \brief VariantCallResult
        \param d
        \param n
    */
    VariantCallResult(UA_Variant *d = nullptr, size_t n = 0) : _data(d), _size(n) {}
    ~VariantCallResult() {
        clear();
    }
    /*!
        \brief clear
    */
    void clear() {
        if (_data) {
            UA_Array_delete(_data, _size, &UA_TYPES[UA_TYPES_VARIANT]);
        }
        _data = nullptr;
        _size = 0;
    }

    /*!
        \brief set
        \param d
        \param n
    */
    void set(UA_Variant *d, size_t n) {
        clear();
        _data = d;
        _size = n;
    }

    /*!
        \brief size
        \return
    */
    size_t size() const {
        return _size;
    }
    /*!
        \brief data
        \return
    */
    UA_Variant *data() const {
        return  _data;
    }

};

/*!
    \brief The VariableAttributes class
*/
class  UA_EXPORT  VariableAttributes : public TypeBase<UA_VariableAttributes> {
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
    void setValue(const Variant &v) {
        UA_Variant_copy(v,  &get().value); // deep copy the variant - do not know life times
    }
    void setValueRank(int i) {
        get().valueRank = i;
    }

    void setHistorizing(bool f = true)
    {
        get().historizing = f;
        if(f)
        {
            get().accessLevel |=  UA_ACCESSLEVELMASK_HISTORYREAD;
        }
        else
        {
            get().accessLevel &= ~UA_ACCESSLEVELMASK_HISTORYREAD;
        }
    }
    void setAccessLevel(UA_Byte b)
    {
        get().accessLevel = b;
    }

    auto& setAccessLevelMask(unsigned char mask) {
      ref()->accessLevel = mask;
      return *this;
    }
    auto& setDataType(NodeId type) {
      ref()->dataType = type;
      return *this;
    }
    VariableAttributes& setArray(const Variant& val);
    VariableAttributes& setHistorizing(bool isHisto = true);
};

/**
 * The attributes for a variable type node. ID: 47
 * @class VariableTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_VariableTypeAttributes struct.
 * Setters are implemented for 2/11 members only
 * No getter, use ->member_name to access them.
 * @todo implement all setters
 * @see UA_VariableTypeAttributes in open62541.h
 */
class UA_EXPORT VariableTypeAttributes : public TypeBase<UA_VariableTypeAttributes> {
public:
    UA_TYPE_DEF(VariableTypeAttributes)
    auto& setDefault() {
        *this = UA_VariableTypeAttributes_default;
        return *this;
    }
    auto& setDisplayName(const std::string& name) {
        ref()->displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.c_str());
        return *this;
    }
    auto& setDescription(const std::string& descr) {
        ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
        return *this;
    }
};

/**
 * The attributes for a method node. ID: 143
 * @class MethodAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_MethodAttributes struct.
 * Setters are implemented for 3/7 members only
 * No getter, use ->member_name to access them.
 * @todo implement all setters
 * @see UA_MethodAttributes in open62541.h
 */
class UA_EXPORT MethodAttributes : public TypeBase<UA_MethodAttributes> {
public:
    UA_TYPE_DEF(MethodAttributes)

    MethodAttributes(const std::string& name);

    auto& setDefault() {
        *this = UA_MethodAttributes_default;
        return *this;
    }
    auto& setDisplayName(const std::string& name) {
        ref()->displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.c_str());
        return *this;
    }
    auto& setDescription(const std::string& descr) {
        ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
        return *this;
    }
    auto& setExecutable(bool exe = true, bool user = true) {
        ref()->executable = exe;
        ref()->userExecutable = user;
        return *this;
    }
};

/**
 * An argument for a method. ID: 132
 * @class ArgumentList open62541objects.h
 * RAII C++ wrapper class for the UA_Argument struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use ->member_name to access them.
 * @see UA_Argument in open62541.h
 */
class UA_EXPORT Argument : public TypeBase<UA_Argument> {
public:
    UA_TYPE_DEF(Argument)
    auto& setDataType(int idx0) {
        ref()->dataType = UA_TYPES[idx0].typeId;
        return *this;
    }
    auto& setDescription(const std::string& descr) {
        ref()->description = UA_LOCALIZEDTEXT_ALLOC("en_US", descr.c_str());
        return *this;
    }
    auto& setName(const std::string& name) {
        ref()->name = UA_STRING_ALLOC(name.c_str());
        return *this;
    }
    auto& setValueRank(int rank) {
        ref()->valueRank = rank;
        return *this;
    }

    UA_Argument& set(
        int type, 
        const std::string& name, 
        const std::string& desc = "", 
        int rank = -1)
    {
        setDataType(type);
        setDescription(name);
        setName(desc);
        setValueRank(rank);
        return get();
    }
};

/**
 * Human readable text with an optional locale identifier.
 * @class LocalizedText open62541objects.h
 * RAII C++ wrapper class for the UA_LocalizedText struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use ->member_name to access them.
 * @see UA_LocalizedText in open62541.h
 */
class UA_EXPORT LocalizedText : public TypeBase<UA_LocalizedText> {
public:
    UA_TYPE_DEF(LocalizedText)
    LocalizedText(const std::string& locale, const std::string& text)
        : TypeBase(UA_LocalizedText_new()) {
        *ref() = UA_LOCALIZEDTEXT_ALLOC(locale.c_str(), text.c_str());
    }
    auto& setLocal(const std::string& language) {
        ref()->locale = UA_STRING_ALLOC(language.c_str());
        return *this;
    }
    auto& setText(const std::string& text) {
        ref()->text = UA_STRING_ALLOC(text.c_str());
        return *this;
    }
};

/**
 * An element in a relative path. ID: 86
 * @class RelativePathElement open62541objects.h
 * RAII C++ wrapper class for the UA_RelativePathElement struct.
 * Setters are implemented for all members,
 * except arrayDimensionsSize and arrayDimensions.
 * No getter, use ->member_name to access them.
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
        ref()->referenceTypeId = typeId.get();
        ref()->isInverse       = includeSubTypes;
        ref()->includeSubtypes = inverse;
        ref()->targetName      = item.get(); // shallow copy!!!
    }
};

/**
 * A relative path constructed from reference types and browse names. ID: 104
 * @class RelativePath open62541objects.h
 * RAII C++ wrapper class for the UA_RelativePath struct.
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
 * @see UABrowsePath in open62541.h
 */
class UA_EXPORT BrowsePath : public TypeBase<UA_BrowsePath> {
public:
    UA_TYPE_DEF(BrowsePath)
    BrowsePath(const NodeId& start, const RelativePath& path) : TypeBase(UA_BrowsePath_new()) {
        UA_RelativePath_copy(path.constRef(), &ref()->relativePath); // deep copy
        UA_NodeId_copy(start, &ref()->startingNode);
    }

    BrowsePath(NodeId& start, RelativePathElement& path) : TypeBase(UA_BrowsePath_new()) {
        ref()->startingNode = start.get();
        ref()->relativePath.elementsSize = 1;
        ref()->relativePath.elements = path.ref();
    }
};

/**
 * The result of a browse operation. ID: 174
 * @class BrowseResult open62541objects.h
 * RAII C++ wrapper class for the UA_BrowseResult struct.
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
 * @todo add accessors for the arrays
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
 * No getter or setter, use ->member_name to access them.
 * @see UA_ViewAttributes in open62541.h
 */
class UA_EXPORT ViewAttributes  : public TypeBase<UA_ViewAttributes> {
public:
    UA_TYPE_DEF(ViewAttributes)
    auto& setDefault() {
        *this = UA_ViewAttributes_default;
        return *this;
    }
};

/**
 * The attributes for a reference type node. ID: 119
 * @class ReferenceTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_ReferenceTypeAttributes struct.
 * No getter or setter, use ->member_name to access them.
 * @see UA_ReferenceTypeAttributes in open62541.h
 */
class UA_EXPORT ReferenceTypeAttributes : public TypeBase< UA_ReferenceTypeAttributes> {
public:
    UA_TYPE_DEF(ReferenceTypeAttributes)
    auto& setDefault() {
        *this = UA_ReferenceTypeAttributes_default;
        return *this;
    }
};

/**
 * The attributes for a data type node. ID: 93
 * @class DataTypeAttributes open62541objects.h
 * RAII C++ wrapper class for the UA_DataTypeAttributes struct.
 * No getter or setter, use ->member_name to access them.
 * @see UA_DataTypeAttributes in open62541.h
 */
class UA_EXPORT DataTypeAttributes : public TypeBase<UA_DataTypeAttributes> {
public:
    UA_TYPE_DEF(DataTypeAttributes)
    auto& setDefault() {
        *this = UA_DataTypeAttributes_default;
        return *this;
    }
};

/**
 * Data Source read and write callbacks.
 * @class DataSource open62541objects.h
 * RAII C++ wrapper class for the UA_DataSource struct.
 * No getter or setter, use ->member_name to access them.
 * @see UA_DataSource in open62541.h
 */
class UA_EXPORT DataSource : public TypeBase<UA_DataSource> {
public:
    DataSource()  : TypeBase(new UA_DataSource()) {
        ref()->read = nullptr;
        ref()->write = nullptr;
    }
};

// Request / Response wrappers for monitored items and events
/**
 * A request to create a subscription. ID: 162
 * @class CreateSubscriptionRequest open62541objects.h
 * RAII C++ wrapper class for the UA_CreateSubscriptionRequest struct.
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
 * @see UA_MonitoredItemCreateRequest in open62541.h
 * @warning not to be confused with CreateMonitoredItemsRequest
 */
class UA_EXPORT MonitoredItemCreateRequest : public TypeBase<UA_MonitoredItemCreateRequest> {
public:
    UA_TYPE_DEF(MonitoredItemCreateRequest)

        void setItem(const NodeId& nodeId, UA_UInt32 attributeId = UA_ATTRIBUTEID_EVENTNOTIFIER, UA_MonitoringMode monitoringMode = UA_MONITORINGMODE_REPORTING)
    {
        get().itemToMonitor.nodeId = nodeId;
        get().monitoringMode = monitoringMode;
        get().itemToMonitor.attributeId = attributeId;
    }
    void setFilter(UA_EventFilter* filter, UA_ExtensionObjectEncoding encoding = UA_EXTENSIONOBJECT_DECODED, const UA_DataType* type = &UA_TYPES[UA_TYPES_EVENTFILTER])
    {
        get().requestedParameters.filter.encoding = encoding;
        get().requestedParameters.filter.content.decoded.data = filter;
        get().requestedParameters.filter.content.decoded.type = type;
    }

    UA_EventFilter* filter() { return (UA_EventFilter*)(get().requestedParameters.filter.content.decoded.data); }
};

/**
 * Response to a Set Monitoring Mode request.  ID: 51
 * @class SetMonitoringModeResponse open62541objects.h
 * RAII C++ wrapper class for the UA_SetMonitoringModeResponse struct.
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
 * No getter or setter, use ->member_name to access them.
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
    UANodeTree(const NodeId& node): _parent(node) {
        root().setData(node);
    }

    virtual ~UANodeTree() {}

    NodeId& parent() {
        return  _parent;
    }

    // client and server have different methods - TO DO unify client and server - and template
    // only deal with value nodes and folders - for now
    virtual bool addFolderNode(
        const NodeId&       parent,
        const std::string&  name,
        NodeId&             newNode= NodeId::Null)  { return false; }

    virtual bool addValueNode(
        const NodeId&       parent,
        const std::string&  name,
        const Variant&      val,
        NodeId&             newNode = NodeId::Null) { return false; }

    virtual bool getValue(const NodeId&, Variant&)  { return false; }
    virtual bool setValue(NodeId&, const Variant&)  { return false; }

    /**
     * Create a path of folder nodes.
     * @param path to build
     * @param node specify the starting node for the path creation
     * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the path.
     * @return true on success.
     */
    bool createPathFolders(const UAPath& path, UANode* node, int level = 0);

    /**
     * Create a path of folder nodes ending with a variable node.
     * @param path to build
     * @param node specify the starting node for the path creation
     * @param val the value of the leaf variable node.
     * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the path.
     * @return true on success.
     */
    bool createPath(const UAPath& path, UANode* node, const Variant& val, int level = 0);

    /**
     * Set the value of a variable node identified by its full path.
     * If the path doesn't exist, build its missing part
     * and create the variable node with the given value.
     * setValue() must be overriden for this to succeed.
     * @param path the full path of the variable node.
     * @param val specify the new value for that node.
     * @return true on success.
     * @see setValue
     */
    bool setNodeValue(const UAPath& path, const Variant& val);

    /**
     * Set the value of a variable node identified by its folder path + its name.
     * If the path doesn't exist, build its missing part
     * and create the variable node with the given name and value.
     * setValue() must be overriden for this to succeed.
     * @param path the folder path of the variable node.
     * @param child the name of the variable node.
     * @param val specify the new value for that node.
     * @return true on success.
     * @see setValue
     */
    bool setNodeValue(UAPath path, const std::string& child, const Variant& val);

    /**
     * Get the value of a variable node identified by its full path, if it exists.
     * getValue() must be overriden for this to succeed.
     * @param path specify the path of the node to retrieve.
     * @param[out] val return the node's value.
     * @return true on success.
     * @see getValue
     */
    bool getNodeValue(const UAPath& path, Variant& val);

    /**
     * Get the value of a variable node identified by its path and name, if it exists.
     * getValue() must be overriden for this to succeed.
     * @param path specify the path of the node to retrieve.
     * @param child the name of the variable node.
     * @param[out] val return the node's value.
     * @return true on success.
     * @see getValue
     */
    bool getNodeValue(UAPath path, const std::string& child, Variant& val);

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
    * @param pNode point on the starting node, can be null.
    * @param os the output stream
    * @param level the number of indentation of the first level. 0 by default.
    */
    void printNode(const UANode* pNode, std::ostream& os = std::cerr, int level = 0);
};

/**
 * Request to create a monitored item. ID: 169
 * @class CreateMonitoredItemsRequest open62541objects.h
 * RAII C++ wrapper class for the UA_CreateMonitoredItemsRequest struct.
 * No getter or setter, use ->member_name to access them.
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
    EventSelectClauseArray(size_t size);
    void setBrowsePath(size_t idx0, const UAPath& path);
    void setBrowsePath(size_t idx0, const std::string& fullPath);
};

typedef std::vector<UAPath> UAPathArray; /**< Events work with sets of browse paths */

/**
 * The EventFilter class. ID: 205
 * @class EventFilter open62541objects.h
 * RAII C++ wrapper class for the UA_EventFilter struct.
 * No getter or setter, use ->member_name to access them.
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
    EventFilterSelect()                     = default;
    EventFilterSelect(size_t size)
        : _selectClause(size)               {}
    ~EventFilterSelect()                    { _selectClause.clear(); }
    EventSelectClauseArray& selectClause()  { return _selectClause; }
    void setBrowsePaths(const UAPathArray& pathArray);
};

/**
 * The RegisteredServer class. ID: 180
 * @class RegisteredServer open62541objects.h
 * RAII C++ wrapper class for the UA_RegisteredServer struct.
 * No getter or setter, use ->member_name to access them.
 * @see UA_RegisteredServer in open62541.h
 */
class UA_EXPORT RegisteredServer : public TypeBase<UA_RegisteredServer> {
    public:
        UA_TYPE_DEF(RegisteredServer)
};

typedef std::unique_ptr<EventFilterSelect> EventFilterRef;

/*!
    \brief EndpointDescriptionArray
*/
typedef Array<UA_EndpointDescription, UA_TYPES_ENDPOINTDESCRIPTION> EndpointDescriptionArray;
/*!
    \brief ApplicationDescriptionArray
*/
typedef Array<UA_ApplicationDescription, UA_TYPES_APPLICATIONDESCRIPTION> ApplicationDescriptionArray;
/*!
    \brief ServerOnNetworkArray
*/
typedef Array<UA_ServerOnNetwork, UA_TYPES_SERVERONNETWORK> ServerOnNetworkArray;

class UA_EXPORT ClientSubscription;
class UA_EXPORT MonitoredItem;
class UA_EXPORT Server;
class UA_EXPORT Client;
class UA_EXPORT ServerRepeatedCallback;

typedef std::list<BrowseItem> BrowseList;

/**
 * The BrowserBase class provide the basic API for browsing list of nodes.
 * Practically an abstract class and should be inherited from to do something.
 */
class UA_EXPORT BrowserBase {
protected:
    BrowseList _list;

   /**
    * A callback used to iteratively process each child nodes.
    * It must match the signature of UA_NodeIteratorCallback,
    * used either by UA_Server_forEachChildNodeCall() or UA_Client_forEachChildNodeCall()
    * @param childId id of the current child to processed.
    * @param isInverse specify if the iteration must be done in reverse (not supported). Use False to iterate normally down the tree.
    * @param referenceTypeId 2nd argument for process(), adding the node's type info.
    * @param handle must point on an instance of a BrowserBase derived class.
    * @return UA_STATUSCODE_GOOD to continue to iterate with next children node, otherwise abort iteration.
    * @see UA_NodeIteratorCallback
    */
    static UA_StatusCode browseIter(
        UA_NodeId   childId,
        UA_Boolean  isInverse,
        UA_NodeId   referenceTypeId,
        void* handle);

public:
    BrowserBase() = default;
    virtual ~BrowserBase()                  { _list.clear(); }
    BrowseList& list()                      { return _list; }

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
        const NodeId& node,
        std::string&  name,
        int&          nsIdx)                { return false; }

   /**
    * Write the content of the list to a given output stream.
    * Each BrowseItem is printed as
    * <nodeId> ns:<nsIdx>: <name> Ref:<refType>\n
    * @param os a reference to the output stream.
    */
    void print(std::ostream& os);

   /**
    * Search the list for a node matching a given name.
    * @param name the browse name of the node to find
    * @return a pointer on the found item, nullptr otherwise.
    */
    BrowseItem* find(const std::string& name);

   /**
    * Populate the _list with the found children nodes.
    * If the given node exists, add its name, namespace,
    * node id and the given reference type in the list of BrowseItem.
    * @param node id of the node to store in the list if it exist.
    * @param referenceTypeId additional info stored in the added BrowseItem.
    */
    void process(const UA_NodeId& node, UA_NodeId referenceTypeId);
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
    Browsable&  _obj;   /**< Must implement browseName() */

public:
    Browser(Browsable& context) : _obj(context) {}
    Browsable& obj() { return _obj; }

    /**
    * Get the name and namespace index of a given node.
    * @param[in] node specify the id of the node to read.
    * @param[out] name the qualified name of the node.
    * @param[out] nsIdx the namespace index of the node.
    * @return true if the node was found. On failure the output param should be unchanged.
    */
    bool browseName(
        const NodeId& node,
        std::string&  name,
        int&          nsIdx) override { // BrowserBase
        return _obj.readBrowseName(node, name, nsIdx);
    }
};


// debug helpers
std::string  timestampToString(UA_DateTime date);
std::string  dataValueToString(UA_DataValue *value);
std::string variantToString(UA_Variant &v);
} // namespace Open62541
#endif // OPEN62541OBJECTS_H
