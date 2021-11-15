/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BYTESTRING_H
#define BYTESTRING_H

#include "open62541/types.h"
#include "open62541/types_generated_handling.h"

namespace Open62541 {

    class ByteString
    {
        UA_ByteString _s;

    public:
        ByteString(const std::string& s) { _s = UA_BYTESTRING_ALLOC(s.c_str()); }

        ByteString(const ByteString& s)
        {
            UA_ByteString_clear(&_s);
            UA_ByteString_copy(&s._s, &_s);
        }

        ByteString(const UA_ByteString& s)
        {
            UA_ByteString_clear(&_s);
            UA_ByteString_copy(&s, &_s);
        }

        ~ByteString() { UA_ByteString_clear(&_s); }

        operator const UA_ByteString&() { return _s; }
        operator const UA_ByteString*() { return &_s; }
        operator UA_ByteString*() { return &_s; }

        ByteString& operator=(const ByteString& s)
        {
            UA_ByteString_clear(&_s);
            UA_ByteString_copy(&s._s, &_s);
            return *this;
        }

        ByteString& operator=(const UA_ByteString& s)
        {
            UA_ByteString_clear(&_s);
            UA_ByteString_copy(&s, &_s);
            return *this;
        }

        std::string toStdString() { return std::string((char*)(_s.data), _s.length); }
    };
}  // namespace Open62541


#endif /* BYTESTRING_H */
