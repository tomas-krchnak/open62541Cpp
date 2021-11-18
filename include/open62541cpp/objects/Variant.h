/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef VARIANT_H
#define VARIANT_H

#include <string>
#include <vector>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/GetUAPrimitiveTypeFunc.h>
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/GetUAPrimitiveTypeFunc.h>
#include <open62541cpp/objects/StringUtils.h>
#include <boost/any.hpp>

namespace Open62541 {

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

class UA_EXPORT Variant : public TypeBase<UA_Variant, UA_TYPES_VARIANT>
{
    /**
    * Configure the variant as a one dimension array.
    * @param size specify the size of the array.
    */
    void set1DArray(size_t size);

public:
        //
    // Construct Variant from ...
    // TO DO add array handling

    explicit Variant()
        : TypeBase(UA_Variant_new())
    {
    }
        
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

    Variant(UA_String& v) : TypeBase(UA_Variant_new()) {
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &v, &UA_TYPES[UA_TYPES_STRING]);
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

        Variant(const char* v)
        : TypeBase(UA_Variant_new())
    {
        UA_String ss = UA_STRING((char*)v);
        UA_Variant_setScalarCopy((UA_Variant*)ref(), &ss, &UA_TYPES[UA_TYPES_STRING]);
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

} // namespace Open62541


#endif /* VARIANT_H */
