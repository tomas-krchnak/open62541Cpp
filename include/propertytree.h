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
#ifndef UA_PROPERTYTREE_H
#define UA_PROPERTYTREE_H
#include <boost/optional/optional.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iterator>
#include <algorithm>
#include <ostream>
#include <functional>

// Mutexs
//
namespace Open62541 {

typedef boost::shared_mutex ReadWriteMutex;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;

// a tree is an addressable set of nodes
// objects of type T must have an assignment operator
//
typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
/**
The NodePath class
*/
template <typename T>
class  NodePath : public std::vector<T> {
public:
    NodePath() {}
    /**
    toList
 * @param s
 * @param seperator
     */
    void toList(const T& s, const char* seperator = ".") {
        boost::char_separator<char> sep(seperator);
        tokenizer tokens(s, sep);
        for (const auto& i : tokens) {
            this->push_back(i);
        }
    }
    /**
    toString
 * @param s
     */
    void toString(T& s) {
        if (this->size() > 0) {
            NodePath& n = *this;
            s = n[0];
            for (unsigned i = 1; i < this->size(); i++) {
                s += ".";
                s += n[i];
            }
        }
    }

    /**
    Append a child path
 * @param p
     */
    const NodePath<T>& append(const NodePath<T>& p) {
        for (const auto& path : p) {
            this->push_back(path);
        }
        return *this;
    }
}; // class  NodePath

template <typename K, typename T>
class Node {
public:
    typedef std::map<K, Node* > ChildMap;
    typedef NodePath<K>         Path;

private:
    K         _name;              //!< the name of the node
    T         _data;              //!< the node data
    Node*     _parent = nullptr;  //!< the node's parent
    ChildMap  _children;          //!<

public:
    class NodeIteratorFunc {
    public:
        virtual void Do(Node*) {}
    };

    /**
    Node
     */
    Node() {}

    /**
    Node
 * @param name
 * @param parent
     */
    Node(const K& name, Node* parent = nullptr)
        : _name(name), _parent(parent) {}

    /**
    ~Node
     */
    virtual ~Node() {
        if (_parent) {
            _parent->_children.erase(name()); // detach
            _parent = nullptr;
        }
        clear();
    }

    /**
    clear
     */
    void clear() {
        for (auto i = _children.begin(); i != _children.end(); i++) {
            Node* n = i->second;
            if (n) {
                n->_parent = nullptr;
                delete n;
            }
        }
        _children.clear();
    }

    /**
    children
 * @return 
     */
    ChildMap& children() {
        return _children;
    }

    /**
    data
 * @return 
     */
    T& data() {
        return _data;
    }

    /**
    setData
 * @param d
     */
    void setData(const T& d) {
        _data = d;
    }

    /**
    child
 * @param s
 * @return 
     */
    Node* child(const K& s) {
        return _children[s];
    }

    /**
    hasChild
 * @param s
 * @return 
     */
    bool hasChild(const K& s) {
        return _children[s] != nullptr;
    }

    /**
    addChild
 * @param key
 * @param n
     */
    void addChild(Node* n) {
        if (hasChild(n->name())) {
            delete _children[n->name()];
            _children.erase(n->name());
        }
        _children[n->name()] = n;
    }

    /**
    createChild
 * @param s
 * @param p
 * @return 
     */
    Node* createChild(const K& s, Node* p = nullptr) {
        if (!p) p = this;
        Node* n = new Node(s, p);
        addChild(n);
        return n;
    }

    /**
    removeChild
 * @param s
     */
    void removeChild(const K& s) {
        if (hasChild(s)) {
            Node* n = child(s);  // take the child node
            _children.erase(s);
            if (n) delete n;
        }
    }
    // accessors
    /**
    name
 * @return 
     */
    const K& name() const {
        return _name;
    }
    /**
    setName
 * @param s
     */
    void setName(const K& s) {
        _name = s;
    }
    /**
    parent
 * @return 
     */
    Node* parent() const {
        return _parent;
    }
    /**
    setParent
 * @param p
     */
    void setParent(Node* p) {
        if (_parent && _parent != p) {
            _parent->_children.erase(name());
        }

        _parent = p;
        if (_parent) _parent->_children[name()] = this;
    }

    /**
    find
 * @param path
 * @param depth
 * @return nullptr on failure
     */
    Node* find(const Path& path, int depth = 0) {
        Node* res = child(path[depth]);// do we have the child at this level?
        if (res) {
            depth++;
            if (depth < (int)path.size()) {
                res = res->find(path, depth);
            }
        }
        return res;
    }


    /**
    find
 * @param path
 * @return 
     */
    Node* find(const K& path) {
        Path p;
        p.toList(path);
        return find(p);
    }

    /**
    add
 * @param p
 * @return 
     */
    Node* add(const Path& p) {
        Node* n = find(p);  // only create if it does not exist
        if (!n) {
            // create the path as required
            n = this;
            int depth = 0;
            while (n->hasChild(p[depth])) {
                n = n->child(p[depth]);
                depth++;
            }
            // create the rest
            for (unsigned i = depth; i < p.size(); i++) {
                n = n->createChild(p[i]);
            }
        }
        //
        return n; // return the newly created node
    }

    /**
    add
 * @param path
 * @return 
     */
    Node* add(const K& path) {
        Path p;
        p.toList(path);
        return add(p);
    }

    /**
    remove
 * @param path
     */
    void remove(const Path& path) {
        if (Node* p = find(path)) {
            delete p;
        }
    }

    /**
    remove
 * @param s
     */
    void remove(const K& s) {
        Path p;
        p.toList(s);
        remove(p);
    }

    /**
    iterateNodes - iterate this node and all children using the given lambda
 * @param func
 * @return 
     */
    bool iterateNodes(std::function<bool (Node&)> func) {
        if (func(*this)) {
            for (auto i = children().begin(); i != children().end(); i++) {
                (i->second)->iterateNodes(func);
            }
            return true;
        }
        return false;
    }

    /**
    iterateNodes
 * @param n
     */
    void iterateNodes(NodeIteratorFunc& n) {
        n.Do(this); // action the function for the node
        for (auto i = children().begin(); i != children().end(); i++) {
            i->second->iterateNodes(n);
        }
    }
    /**
    write
 * @param os
     */
    template <typename STREAM>
    void write(STREAM& os) {
        os << name();
        os << data();
        os << static_cast<int>(children().size()); // this node's data
                                                   // now recurse
        if (children().size() > 0) {
            for (auto i = children().begin(); i != children().end(); i++) {
                i->second->write(os);
            }
        }
    }


    /**
    read
 * @param is
     */
    template <typename STREAM> void read(STREAM& is) {
        int n = 0;
        clear();
        is >> _name;
        is >> _data;
        is >> n;
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                Node* o = new Node();
                o->read(is); // recurse
                addChild(o); // add subtree to children
            }
        }
    }


    /**
    copyTo
    recursive copy
 * @param n
     */
    void copyTo(Node* n) {
        n->clear();
        n->setName(name());
        n->setData(data());
        if (children().size() > 0) {
            for (auto i = children().begin(); i != children().end(); i++) {
                Node* c = new Node();
                n->addChild(c); // add the child
                i->second->copyTo(c); // now recurse
            }
        }
    }
}; // class Node

/**

*/
template <typename K, typename T>
class PropertyTree {
    mutable ReadWriteMutex  _mutex;
    bool                    _changed = false;

public:
    T                   _defaultData;
    typedef Node<K, T>  PropertyNode;
    typedef NodePath<K> Path;

private:
    PropertyNode _empty;  //!< empty node
    PropertyNode _root;   //!< the root node

public:
    /**
    PropertyTree
     */
    PropertyTree() :
        _empty("__EMPTY__"),
        _root("__ROOT__") {
        _root.clear();
    }
    /**
    ~PropertyTree
     */
    virtual ~PropertyTree()         { _root.clear(); }

    /**
    mutex
 * @return 
     */
    ReadWriteMutex& mutex()         { return _mutex; }
    /**
    changed
 * @return 
     */
    bool changed()            const { return _changed; }
    /**
    clearChanged
     */
    void clearChanged()             { _changed = false; }
    /**
    setChanged
 * @param f
     */
    void setChanged(bool f = true)  { _changed = f; }
    /**
    clear
     */
    void clear() {
        WriteLock l(_mutex);
        _root.clear();
        setChanged();
    }

    /**

     */
    template <typename P>
    T& get(const P& path) {
        ReadLock l(_mutex);
        if (auto* p = _root.find(path)) {
            return p->data();
        }
        return _defaultData;
    }

    /**
    root
 * @return 
     */
    PropertyNode& root()      { return _root; }

    /**
    rootNode
 * @return 
     */
    PropertyNode* rootNode()  { return &this->_root; }

    /**
    node
     */
    template <typename P>
    PropertyNode* node(const P& path) {
        ReadLock l(_mutex);
        return  _root.find(path);
    }

    /**
    set
     */
    template <typename P>
    PropertyNode* set(const P& path, const T& d) {
        auto p = _root.find(path);
        if (!p) {
            WriteLock l(_mutex);
            p =  _root.add(path);
        }
        if (p) {
            WriteLock l(_mutex);
            p->setData(d);
        }
        setChanged();
        return p;
    }

    /**
    exists
     */
    template <typename P>
    bool exists(const P& path) {
        return _root.find(path) != nullptr;
    }

    /**

     */
    template <typename P>
    void remove(const P& path) {
        WriteLock l(_mutex);
        setChanged();
        _root.remove(path);
    }

    /**
    absolutePath
 * @param n
 * @param p
     */
    void absolutePath(PropertyNode* n, Path& p) {
        p.clear();
        if (n) {
            ReadLock l(_mutex);
            do
            {
                p.push_back(n->name());
                n = n->parent();
            } while (n != nullptr);
            std::reverse(std::begin(p), std::end(p));
        }
    }

    /**
    getChild
 * @param node
 * @param s
 * @param def
 * @return 
     */
    T& getChild(PropertyNode* node, const K& s, T& def) {
        ReadLock l(_mutex);
        if (node && node->hasChild(s)) {
            return node->child(s)->data();
        }
        return def;
    }

    /**
    setChild
 * @param node
 * @param s
 * @param v
     */
    void  setChild(PropertyNode* node, const K& s, const T& v) {
        if (node) {
            WriteLock l(_mutex);
            if (node->hasChild(s)) {
                node->child(s)->setData(v);
            }
            else {
                PropertyNode* c = node->createChild(s);
                c->setData(v);
            }
            setChanged();
        }
    }

    /**
    iterateNodes
 * @param func
 * @return 
     */
    bool iterateNodes(std::function<bool (PropertyNode&)> func) {
        WriteLock l(_mutex);
        return _root.iterateNodes(func);
    }

    /**
    write
     */
    template <typename S> void write(S& os) {
        ReadLock l(_mutex);
        _root.write(os);
    }

    /**
    read
     */
    template <typename S> void read(S& is) {
        WriteLock l(_mutex);
        _root.read(is);
        setChanged();
    }

    /**
    copyTo
 * @param dest
     */
    void copyTo(PropertyTree& dest) {
        ReadLock l(_mutex);
        _root.copyTo(&dest._root);
        dest.setChanged();
    }

    /**
    list
 * @param path
 * @param l
     */
    template <typename P>
    int listChildren(const P& path, std::vector<K>& l) {
        auto i =  node(path);
        if (i) {
            ReadLock lx(_mutex);
            for (auto j = i->children().begin(); j != i->children().end(); j++) {
                l.push_back(j->first);
            }
        }
        return l.size();
    }
}; // class PropertyTree

} // namespace Open62541

#endif // PROPERTYTREE_H
