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
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/propertytree.h>

namespace Open62541 {

    /**
     * A thread-safe tree used to have nodes in a browsable / addressable way.
     */
    typedef NodePath<std::string> UAPath;
    typedef PropertyTree<std::string, NodeId>::PropertyNode UANode;
    //
    /*!
        \brief The UANodeTree class
    */
    class UA_EXPORT UANodeTree : public PropertyTree<std::string, NodeId>
    {
        NodeId _parent;  // note parent node

    public:
        UANodeTree(const NodeId& node)
            : _parent(node)
        {
            root().setData(node);
        }

        virtual ~UANodeTree() {}

        NodeId& parent() { return _parent; }

        // client and server have different methods - TO DO unify client and server - and template
        // only deal with value nodes and folders - for now
        virtual bool addFolderNode(const NodeId& parent, const std::string& name, NodeId& newNode = NodeId::Null)
        {
            return false;
        }

        virtual bool addValueNode(const NodeId& parent,
                                  const std::string& name,
                                  const Variant& val,
                                  NodeId& newNode = NodeId::Null)
        {
            return false;
        }

        virtual bool getValue(const NodeId&, Variant&) { return false; }
        virtual bool setValue(NodeId&, const Variant&) { return false; }

        /**
         * Create a path of folder nodes.
         * @param path to build
         * @param node specify the starting node for the path creation
         * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the
         * path.
         * @return true on success.
         */
        bool createPathFolders(const UAPath& path, UANode* node, int level = 0);

        /**
            \brief createPathFolders
         * @param path to build
         * @param node specify the starting node for the path creation
         * @param val the value of the leaf variable node.
         * @param level specify the index in the path of the starting node. Permit to skip folder at the begining of the
         path.
         * @return true on success.
         */
        bool createPath(const UAPath& path, UANode* node, const Variant& val, int level = 0);

        /*!
         * If the path doesn't exist, build its missing part
         * and create the variable node with the given value.
         * setValue() must be overriden for this to succeed.
         * @param path the full path of the variable node.
         * @param val specify the new value for that node.
         * @return true on success.
         * @see setValue
         */
        bool setNodeValue(const UAPath& path, const Variant& val);

        /**
            \brief setNodeValue
         * If the path doesn't exist, build its missing part
         * and create the variable node with the given name and value.
         * setValue() must be overriden for this to succeed.
         * @param path the folder path of the variable node.
         * @param child the name of the variable node.
         * @param val specify the new value for that node.
         * @return true on success.
         * @see setValue
         */
        bool setNodeValue(UAPath path, const std::string& child, const Variant& val);

        /**
         * Get the value of a variable node identified by its full path, if it exists.
         * getValue() must be overriden for this to succeed.
         * @param path specify the path of the node to retrieve.
         * @param[out] val return the node's value.
         * @return true on success.
         * @see getValue
         */
        bool getNodeValue(const UAPath& path, Variant& val);

        /**
            \brief printNode
         * getValue() must be overriden for this to succeed.
         * @param path specify the path of the node to retrieve.
         * @param child the name of the variable node.
         * @param[out] val return the node's value.
         * @return true on success.
         * @see getValue
         */
        bool getNodeValue(UAPath path, const std::string& child, Variant& val);

        /**
         * Write the descendant tree structure of a node to an output stream.
         * The node name and data are written indented according to their degree in the tree hierarchy.
         * nd1
         *   nd11
         *     nd111
         *   nd12
         *     nd121
         *       nd1211
         *       nd1212
         *     nd122
         *     nd123
         *   nd13
         *     nd131
         * @param pNode point on the starting node, can be null.
         * @param os the output stream
         * @param level the number of indentation of the first level. 0 by default.
         */
        void printNode(const UANode* pNode, std::ostream& os = std::cerr, int level = 0);
    };
} // namespace Open62541
