/*
 * Copyright (C) 2017 -  B. J. Hill
 *
 * This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
 * redistribute it and/or modify it under the terms of the Mozilla Public
 * License v2.0 as stated in the LICENSE file provided with open62541.
 *
 * open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 */
#include "open62541client.h"
#include "clientbrowser.h"

namespace Open62541 {

void Client::subscriptionInactivityCallback(UA_Client *client, UA_UInt32 subscriptionId, void *subContext)
{
    if(auto p = (Client*)UA_Client_getContext(client)) {
        p->subscriptionInactivity(subscriptionId, subContext);
    }
}

void  Client::asyncServiceCallback(UA_Client *client, void *userdata,
                                 UA_UInt32 requestId, void *response,
                                 const UA_DataType *responseType)
{
    if(auto p = (Client *)UA_Client_getContext(client)) {
       p->asyncService(userdata, requestId, response, responseType);
    }
}

void  Client::stateCallback (UA_Client *client, UA_ClientState clientState)
{
    if(auto p =   (Client *)(UA_Client_getContext(client))) {
        p->stateChange(clientState);
    }
}

bool Client::deleteTree(NodeId &nodeId) {
    if (!_client)
        return lastOK();
    
    NodeIdMap m;
    browseTree(nodeId, m);
    for (auto i = m.begin(); i != m.end(); i++) {
        UA_NodeId &ni =  i->second;
        if (ni.namespaceIndex > 0) { // namespace 0 appears to be reserved
            WriteLock l(_mutex);
            UA_Client_deleteNode(_client, i->second, true);
        }
    }
    
    return lastOK();
}

/**
 * browseTreeCallBack
 * @param childId
 * @param isInverse
 * @param referenceTypeId
 * @param handle
 * @return 
 */
static UA_StatusCode browseTreeCallBack(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId /*referenceTypeId*/, void *handle) {
    if (!isInverse) { // not a parent node - only browse forward
        auto pl = (UANodeIdList*)handle;
        pl->put(childId);
    }
    return UA_STATUSCODE_GOOD;
}

bool Client::browseChildren(UA_NodeId &nodeId, NodeIdMap &m) {
    UANodeIdList l;
    {
        WriteLock ll(mutex());
        UA_Client_forEachChildNodeCall(_client, nodeId,  browseTreeCallBack, &l); // get the childlist
    }
    for (int i = 0; i < int(l.size()); i++) {
        if (l[i].namespaceIndex == nodeId.namespaceIndex) { // only in same namespace
            std::string s = toString(l[i]);
            if (m.find(s) == m.end()) {
                m.put(l[i]);
                browseChildren(l[i], m); // recurse no duplicates
            }
        }
    }
    return lastOK();
}

bool Client::browseTree(NodeId &nodeId, UANodeTree &tree) {
    // form a heirachical tree of nodes given node is added to tree
    tree.root().setData(nodeId); // set the root of the tree
    return browseTree(nodeId.get(), tree.rootNode());
}

bool Client::browseTree(UA_NodeId &nodeId, UANode *node) {
    // form a heirachical tree of nodes
    if(_client)
    {
        UANodeIdList l;
        {
            WriteLock ll(mutex());
            UA_Client_forEachChildNodeCall(_client, nodeId,  browseTreeCallBack, &l); // get the childlist
        }
        for (int i = 0; i < int(l.size()); i++) {
            if (l[i].namespaceIndex > 0) {
                QualifiedName outBrowseName;
                {
                    WriteLock ll(mutex());
                    _lastError = __UA_Client_readAttribute(_client, &l[i], UA_ATTRIBUTEID_BROWSENAME, outBrowseName, &UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
                }
                if (lastOK()) {
                    std::string s = toString(outBrowseName.get().name); // get the browse name and leaf key
                    NodeId nId = l[i]; // deep copy
                    UANode *n = node->createChild(s); // create the node
                    n->setData(nId);
                    browseTree(l[i], n);
                }
            }
        }
    }
    return lastOK();
}

bool Client::browseTree(NodeId &nodeId, NodeIdMap &m) {
    m.put(nodeId);
    return browseChildren(nodeId, m);
}

UA_StatusCode Client::getEndpoints(const std::string &serverUrl, std::vector<std::string> &list) {
    if (_client) {
        UA_EndpointDescription *endpointDescriptions = nullptr;
        size_t endpointDescriptionsSize = 0;

        {
            WriteLock l(_mutex);
            _lastError = UA_Client_getEndpoints(_client, serverUrl.c_str(), &endpointDescriptionsSize, &endpointDescriptions);
        }
        if (_lastError == UA_STATUSCODE_GOOD) {
            for (int i = 0; i < int(endpointDescriptionsSize); i++) {

                list.push_back(toString(endpointDescriptions[i].endpointUrl));
            }
        }
        return _lastError;
    }
    throw std::runtime_error("Null client");
    return 0;
}

bool Client::nodeIdFromPath(NodeId &start, Path &path, NodeId &nodeId) {
    // nodeId is a shallow copy - do not delete and is volatile
    UA_NodeId n = start.get();

    int level = 0;
    if (path.size() > 0) {
        ClientBrowser b(*this);
        while (level < int(path.size())) {
            b.browse(n);
            auto i = b.find(path[level]);
            if (i == b.list().end()) return false;
            level++;
            n = (*i).childId;
        }
    }

    nodeId = n; // deep copy
    return level == int(path.size());
}

bool Client::createFolderPath(NodeId &start, Path &path, int nameSpaceIndex, NodeId &nodeId) {
    //
    // create folder path first then add variables to path's end leaf
    //
    UA_NodeId n = start.get();
    //
    int level = 0;
    if (path.size() > 0) {
        ClientBrowser b(*this);
        while (level < int(path.size())) {
            b.browse(n);
            auto i = b.find(path[level]);
            if (i == b.list().end())  break;
            level++;
            n = (*i).childId; // shallow copy
        }
        if (level == int(path.size())) {
            nodeId = n;
        }
        else {
            NodeId nf(nameSpaceIndex, 0); // auto generate NODE id
            nodeId = n;
            NodeId newNode;
            while (level < int(path.size())) {
                addFolder(nodeId, path[level], nf, newNode.notNull(), nameSpaceIndex);
                if (!lastOK()) {
                    break;
                }
                nodeId = newNode; // assign
                level++;
            }
        }
    }
    return level == int(path.size());
}

bool Client::getChild(NodeId &start, const std::string &childName, NodeId &ret) {
    Path p;
    p.push_back(childName);
    return nodeIdFromPath(start, p, ret);
}

bool Client::addFolder(NodeId &parent,  const std::string &childName,
                                  NodeId &nodeId,  NodeId &newNode, int nameSpaceIndex) {
    if(!_client)
      return false;

    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
      nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    QualifiedName qn(nameSpaceIndex, childName);
    ObjectAttributes attr;
    attr.setDisplayName(childName);
    attr.setDescription(childName);
    _lastError = UA_Client_addObjectNode(
        _client,
        nodeId,
        parent,
        NodeId::Organizes,
        qn,
        NodeId::FolderType,
        attr.get(),
        newNode.isNull()?nullptr:newNode.ref());

    return lastOK();
}

bool Client::addVariable(
  NodeId &parent,
  const std::string &childName,
  const Variant &value,
  NodeId &nodeId,
  NodeId &newNode,
  int nameSpaceIndex)
{
    if(!_client)
      return false;

    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
      nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    QualifiedName qn(nameSpaceIndex, childName);
    var_attr.setDisplayName(childName);
    var_attr.setDescription(childName);
    var_attr.setValue(value);
    _lastError = UA_Client_addVariableNode(
        _client,
        nodeId, // Assign new/random NodeID
        parent,
        NodeId::Organizes,
        qn,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        var_attr,
        newNode.isNull()?nullptr:newNode.ref());
    
    return lastOK();
}

bool Client::addProperty(
  NodeId &parent,
  const std::string &key,
  Variant &value,
  NodeId &nodeId,
  NodeId &newNode,
  int nameSpaceIndex)
{
    if(!_client)
      return false;

    WriteLock l(_mutex);
    if (nameSpaceIndex == 0)
      nameSpaceIndex = parent.nameSpaceIndex(); // inherit parent by default

    VariableAttributes var_attr;
    QualifiedName qn(nameSpaceIndex, key);
    var_attr.setDisplayName(key);
    var_attr.setDescription(key);
    var_attr.setValue(value);
    _lastError = UA_Client_addVariableNode(
        _client,
        nodeId, // Assign new/random NodeID
        parent,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        qn,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // no variable type
        var_attr,
        newNode.isNull()?nullptr:newNode.ref());

    return lastOK();
}

} // namespace Open62541

