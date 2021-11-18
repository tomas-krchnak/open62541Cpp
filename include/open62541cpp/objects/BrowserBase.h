/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef BROWSERBASE_H
#define BROWSERBASE_H

#include <string>
#include <list>
#include "open62541/types.h"
#include <open62541cpp/objects/BrowseItem.h>
#include <open62541cpp/objects/NodeId.h>

namespace Open62541 {

    typedef std::list<BrowseItem> BrowseList;

        /*!
        \brief The BrowserBase class
     * The BrowserBase class provide the basic API for browsing list of nodes.
     * Practically an abstract class and should be inherited from to do something.
     */
    class UA_EXPORT BrowserBase
    {
    protected:
        BrowseList _list;
        /**
         * A callback used to iteratively process each child nodes.
         * It must match the signature of UA_NodeIteratorCallback,
         * used either by UA_Server_forEachChildNodeCall() or UA_Client_forEachChildNodeCall()
         * @param childId id of the current child to processed.
         * @param isInverse specify if the iteration must be done in reverse (not supported). Use False to iterate normally
         * down the tree.
         * @param referenceTypeId 2nd argument for process(), adding the node's type info.
         * @param handle must point on an instance of a BrowserBase derived class.
         * @return UA_STATUSCODE_GOOD to continue to iterate with next children node, otherwise abort iteration.
         * @see UA_NodeIteratorCallback
         */
        static UA_StatusCode browseIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void* handle);

    public:
        BrowserBase() = default;
        virtual ~BrowserBase() { _list.clear(); }
        BrowseList& list() { return _list; }

        /**
         * Browse from a starting node.
         * Must be overridden to do anything
         * @param start the id of the browsing starting node
         */
        virtual void browse(UA_NodeId start) {}

        /**
         * Get the name and namespace index of a given node.
         * Should be customized by derived class.
         * @param[in] node specify the nodeId of the node to read
         * @param[out] name the qualified name of the node
         * @param[out] nsIdx the namespace index of the node
         * @return true if the node was found. On failure the output param should be unchanged.
         */
        virtual bool browseName(const NodeId& node, std::string& name, int& nsIdx) { return false; }

        /**
         * Write the content of the list to a given output stream.
         * Each BrowseItem is printed as
         * <nodeId> ns:<nsIdx>: <name> Ref:<refType>\n
         * @param os a reference to the output stream.
         */
        void print(std::ostream& os);
        /*!
       /**
        * Search the list for a node matching a given name.
        * @param name the browse name of the node to find
        * @return a pointer on the found item, nullptr otherwise.
        */
        BrowseItem* find(const std::string& name);
        /*!
       /**
        * Populate the _list with the found children nodes.
        * If the given node exists, add its name, namespace,
        * node id and the given reference type in the list of BrowseItem.
        * @param node id of the node to store in the list if it exist.
        * @param referenceTypeId additional info stored in the added BrowseItem.
        */
        void process(const UA_NodeId& node, UA_NodeId referenceTypeId);
    };

}  // namespace Open62541


#endif /* BROWSERBASE_H */
