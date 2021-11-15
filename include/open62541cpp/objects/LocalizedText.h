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
#include "open62541/types_generated_handling.h"

namespace Open62541 {
    /**
     * Human readable text with an optional locale identifier.
     * @class LocalizedText open62541objects.h
     * RAII C++ wrapper class for the UA_LocalizedText struct.
     * Setters are implemented for all members,
     * except arrayDimensionsSize and arrayDimensions.
     * No getter, use ->member_name to access them.
     * @see UA_LocalizedText in open62541.h
     */
class UA_EXPORT LocalizedText : public TypeBase<UA_LocalizedText, UA_TYPES_LOCALIZEDTEXT>
    {
    public:
        LocalizedText(const std::string& locale, const std::string& text)
            : TypeBase(UA_LocalizedText_new())
        {
            *ref() = UA_LOCALIZEDTEXT_ALLOC(locale.c_str(), text.c_str());
        }
        auto& setLocal(const std::string& language)
        {
            ref()->locale = UA_STRING_ALLOC(language.c_str());
            return *this;
        }
        auto& setText(const std::string& text)
        {
            ref()->text = UA_STRING_ALLOC(text.c_str());
            return *this;
        }
    };
} // namespace Open62541
