/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BROWSABLETEMPLATE_H
#define BROWSABLETEMPLATE_H

#include <string>
#include "open62541/types.h"
#include <open62541cpp/objects/BrowserBase.h>
#include <open62541cpp/objects/NodeId.h>

namespace Open62541 {
    /**
     * Template class permitting to customize what is browsed.
     * @param Browsable a class having a browseName() method matching
     * the signature of BrowserBase::browseName().
     * browse() should be overriden to do something.
     * browseName() is customized by the Browsable::browseName().
     */
    template <typename Browsable>
class Browser : public BrowserBase
    {
        Browsable& _obj; /**< Must implement browseName() */

    public:
        Browser(Browsable& context)
            : _obj(context)
        {
        }
        Browsable& obj() { return _obj; }

        /**
         * Get the name and namespace index of a given node.
         * @param[in] node specify the id of the node to read.
         * @param[out] name the qualified name of the node.
         * @param[out] nsIdx the namespace index of the node.
         * @return true if the node was found. On failure the output param should be unchanged.
         */
        bool browseName(const NodeId& node, std::string& name, int& nsIdx) override
        {  // BrowserBase
            return _obj.readBrowseName(node, name, nsIdx);
        }
    };
}  // namespace Open62541


#endif /* BROWSABLETEMPLATE_H */
