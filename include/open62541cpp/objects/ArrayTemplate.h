/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef ARRAYTEMPLATE_H
#define ARRAYTEMPLATE_H

#include <string>
#include <vector>
#include "open62541/types.h"
#include "open62541/types_generated.h"

namespace Open62541 {

template <typename T, const UA_UInt32 I>
/*!
       \brief The Array class
       This is for allocated arrays of UA_ objects
       simple lifecycle management.
       Uses UA_array_new and UA_array_delete
       rather than new and delete
       Also deals with arrays returned from UA_ functions
       The optional initFunc and clearFunc parameters are the UA initialise and clear functions for the structure
*/

class Array
{
    T* _data       = nullptr;
    size_t _length = 0;

public:
    Array() { release(); }
    Array(T* data, size_t len)
        : _data(data)
        , _length(len)
    {
        // shallow copy
    }

    Array(size_t n)
    {
        if (n > 0) {
            allocate(n);
            prepare();
        }
    }

    virtual ~Array() { clear(); }

    /**
     * detach and transfer ownership to the caller - no longer managed
     */
    const UA_DataType* dataType() { return &UA_TYPES[I]; }
    /*!
        \brief allocate
        \param len
    */
    void allocate(size_t len)
    {
        clear();
        _data   = (T*)(UA_Array_new(len, dataType()));
        _length = len;
    }

    /*!
        \brief release
        detach and transfer ownership to the caller - no longer managed
    */
    void release()
    {
        _length = 0;
        _data   = nullptr;
    }

    virtual void clearFunc(T*) {}
    /*!
        \brief clear
    */
    auto& clear()
    {

        if (m_size && m_pData) {
            T* p = m_pData;
            for (size_t i = 0; i < m_size; i++, p++) {
                clearFunc(p);
            }
            UA_Array_delete(m_pData, m_size, &UA_TYPES[I]);
        }
        m_size  = 0;
        m_pData = nullptr;
        return *this;
    }

    /*!
        \brief at
        \return
    */
    T& at(size_t i) const
    {
        if (!_data || (i >= _length))
            throw std::exception();
        return _data[i];
    }

    /*!
    \brief setList
    \param len
    \param data
*/
    auto& setList(size_t len, T* data)
    {
        clear();
        m_size  = len;
        m_pData = data;
        return *this;
    }

    // Accessors
    size_t length() const { return _length; }
    T* data() const { return _data; }
    //
    size_t* lengthRef() { return &_length; }
    T** dataRef() { return &_data; }
    //
    operator T*() { return _data; }
    //
    virtual void initFunc(T*) {}
    void prepare()
    {
        if (_length > 0) {
            T* p = _data;
            for (size_t i = 0; i < _length; i++, p++) {
                initFunc(p);
            }
        }
    }

    // Iterator: so Array is usable in range for loop
    class iterator
    {
        T* ptr;

    public:
        iterator(T* ptr)
            : ptr(ptr)
        {
        }
        iterator operator++()
        {
            ++ptr;
            return *this;
        }
        bool operator!=(const iterator& o) const { return ptr != o.ptr; }
        const T& operator*() const { return *ptr; }
    };

    iterator begin() const { return iterator(m_pData); }
    iterator end() const { return iterator(m_pData + m_size); }

};  // class Array

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
typedef std::vector<std::string> StdStringArray;

//
// typedef basic array types
// macro to define the init and clear functions for the complex objects
#define ARRAY_INIT_CLEAR(T)                \
    void clearFunc(T* p) { T##_clear(p); } \
    void initFunc(T* p) { T##_init(p); }

// Generate an array declaration with the type specific constructors and destructors - has to the use the UA versions
#define TYPEDEF_ARRAY(T, I)                 \
    typedef Array<UA_##T, I> T##Array_Base; \
    class T##Array : public T##Array_Base   \
    {                                       \
    public:                                 \
        T##Array() {}                       \
        T##Array(UA_##T* data, size_t len)  \
            : T##Array_Base(data, len)      \
        {                                   \
        }                                   \
        T##Array(size_t n)                  \
            : T##Array_Base(n)              \
        {                                   \
        }                                   \
        ARRAY_INIT_CLEAR(UA_##T)            \
    };

TYPEDEF_ARRAY(String, UA_TYPES_STRING)
TYPEDEF_ARRAY(NodeId, UA_TYPES_NODEID)
TYPEDEF_ARRAY(QualifiedName, UA_TYPES_QUALIFIEDNAME)

}  // namespace Open62541


#endif /* ARRAYTEMPLATE_H */
