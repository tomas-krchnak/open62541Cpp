/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include "open62541/plugin/accesscontrol_default.h"

namespace Open62541 {

    /**
     * Default access control. The log-in can be anonymous or username-password.
     * A logged-in user has all access rights.
     * @class UsernamePasswordLogin open62541objects.h
     * RAII C++ wrapper class for the UA_UsernamePasswordLogin struct
     * Setters are implemented for all member.
     * No getter, use ->member_name to access them.
     * public members are username and password UA_String
     */
class UA_EXPORT UsernamePasswordLogin : public TypeBase<UA_UsernamePasswordLogin, UNKNOWN_UA_TYPE>
    {
    public:
        UsernamePasswordLogin(const std::string& u = "", const std::string& p = "")
            : TypeBase(new UA_UsernamePasswordLogin())
        {
            UA_String_init(&ref()->username);
            UA_String_init(&ref()->password);
            setUserName(u);
            setPassword(p);
        }

        ~UsernamePasswordLogin()
        {
            UA_String_deleteMembers(&ref()->username);
            UA_String_deleteMembers(&ref()->password);
        }

        UsernamePasswordLogin& setUserName(const std::string& str)
        {
            fromStdString(str, ref()->username);
            return *this;
        }

        UsernamePasswordLogin& setPassword(const std::string& str)
        {
            fromStdString(str, ref()->password);
            return *this;
        }
    };

} // namespace Open62541
