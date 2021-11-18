/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef VARIABLEATTRIBUTES_H
#define VARIABLEATTRIBUTES_H

#include <string>
#include "open62541/types.h"
#include "open62541/types_generated.h"
#include "open62541/server.h"
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/objects/Variant.h>

namespace Open62541 {
    /*!
        \brief The VariableAttributes class
    */
    class UA_EXPORT VariableAttributes : public TypeBase<UA_VariableAttributes, UA_TYPES_VARIABLEATTRIBUTES>
    {
    public:
        VariableAttributes();
        VariableAttributes(const std::string& name, const Variant& value);
        using TypeBase<UA_VariableAttributes, UA_TYPES_VARIABLEATTRIBUTES>::operator=;
        void setDefault() { *this = UA_VariableAttributes_default; }

        // feat: Add member function for array dimension and size
        template <typename T>
        Variant getVariantMatrix(const UA_UInt32 rows,
                                 const UA_UInt32 cols,
                                 const size_t dim_size,
                                 const UA_DataType* type,
                                 const int value_rank,
                                 const T* array)
        {
            *this = UA_VariableAttributes_default;
            Variant variant;
            // set the VariableAttribute values' constraints
            get().valueRank           = value_rank;
            get().dataType            = type->typeId;
            get().arrayDimensions     = (UA_UInt32*)UA_Array_new(dim_size, type);
            get().arrayDimensionsSize = dim_size;
            get().arrayDimensions[0]  = rows;
            get().arrayDimensions[1]  = cols;

            // Set the value from the argument array. The array dimensions need to be the same for the value
            size_t arraySize = sizeof(*array) / sizeof(array);
            UA_Variant_setArrayCopy(&get().value, array, arraySize, type);
            get().value.arrayDimensions     = (UA_UInt32*)UA_Array_new(dim_size, type);
            get().value.arrayDimensionsSize = dim_size;
            get().value.arrayDimensions[0]  = rows;
            get().value.arrayDimensions[1]  = cols;
            UA_Variant_copy(&get().value, variant);
            return variant;
        }

        void setDisplayName(const std::string& s) { get().displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str()); }
        void setDescription(const std::string& s) { get().description = UA_LOCALIZEDTEXT_ALLOC("en_US", s.c_str()); }
        void setValue(const Variant& v)
        {
            UA_Variant_copy(v, &get().value);  // deep copy the variant - do not know life times
        }
        void setValueRank(int i) { get().valueRank = i; }

        auto& setAccessLevelMask(unsigned char mask)
        {
            ref()->accessLevel = mask;
            return *this;
        }
        auto& setDataType(NodeId type)
        {
            ref()->dataType = type;
            return *this;
        }
        VariableAttributes& setArray(const Variant& val);
        VariableAttributes& setHistorizing(bool isHisto = true);
    };

} // namespace Open62541


#endif /* VARIABLEATTRIBUTES_H */
