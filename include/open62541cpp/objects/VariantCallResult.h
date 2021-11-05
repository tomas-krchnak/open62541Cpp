/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include "open62541/types.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>

namespace Open62541 {
    // Wrap method call return value sets
    /*!
        \brief The VariantCallResult class
    */
    // this takes over management of the returned data
    class UA_EXPORT VariantCallResult
    {
        UA_Variant* _data = nullptr;
        size_t _size      = 0;

    public:
        /*!
            \brief VariantCallResult
            \param d
            \param n
        */
        VariantCallResult(UA_Variant* d = nullptr, size_t n = 0)
            : _data(d)
            , _size(n)
        {
        }
        ~VariantCallResult() { clear(); }
        /*!
            \brief clear
        */
        void clear()
        {
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
        void set(UA_Variant* d, size_t n)
        {
            clear();
            _data = d;
            _size = n;
        }

        /*!
            \brief size
            \return
        */
        size_t size() const { return _size; }
        /*!
            \brief data
            \return
        */
        UA_Variant* data() const { return _data; }
    };
} // namespace Open62541
