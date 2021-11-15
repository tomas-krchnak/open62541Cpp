/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef STRING_H
#define STRING_H

#include "open62541/types.h"
#include "open62541/types_generated_handling.h"

namespace Open62541 {

    /*!
     * \brief The String class
     */
    class String
    {
        UA_String _s;

    public:
        String(const std::string& s) { _s = UA_String_fromChars(s.c_str()); }

        String(const String& s)
        {
            UA_String_clear(&_s);
            UA_String_copy(&s._s, &_s);
        }

        String(const UA_String& s)
        {
            UA_String_clear(&_s);
            UA_String_copy(&s, &_s);
        }

        ~String() { UA_String_clear(&_s); }

        operator const UA_String&() { return _s; }
        operator const UA_String*() { return &_s; }
        operator UA_String*() { return &_s; }

        String& operator=(const String& s)
        {
            UA_String_clear(&_s);
            UA_String_copy(&s._s, &_s);
            return *this;
        }

        String& operator=(const UA_String& s)
        {
            UA_String_clear(&_s);
            UA_String_copy(&s, &_s);
            return *this;
        }

        std::string toStdString() { return std::string((char*)(_s.data), _s.length); }
    };
}  // namespace Open62541


#endif /* STRING_H */
