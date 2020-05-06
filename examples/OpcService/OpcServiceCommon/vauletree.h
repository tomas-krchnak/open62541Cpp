#ifndef VALUETREE_H
#define VALUETREE_H

#include "trace.hpp"
#include "propertytree.h"
#include <Wt/Json/Object>
#include <Wt/Json/Value>

namespace MRL {

/**
 * The ValueTree class
 */
template <typename V>
class ValueTree : public PropertyTree<std::string, V> {

public:

    typedef  Node<std::string, V> ValueNode;
    typedef NodePath<std::string> ValuePath;

    /**
     * VariantPropertyTree
     */
    ValueTree() {}
    virtual ~ValueTree() {}

    /**
     * setValue
     * @param path
     * @param v
     */
    template <typename P, typename T>
    void setValue(P path, const T& v) {
        T a(v);
        this->set(path, a);
    }

    /**
     * setValue
     * @param path
     * @param v
     */
    template <typename T>
    void setValue(MRL::PropertyPath& path, const std::string& c, const T& v) {
        if (!c.empty()) {
            path.push_back(c);
            V a(v);
            this->set(path, a);
            path.pop_back();
        }
    }

    /**
     * getAsWxString
     * @param path
     * @return
     */
    template<typename P>
    std::string getAsString(P path) {
        try {
            ReadLock l(this->mutex());
            V& a = this->get(path);
            if (!a.empty()) {
                std::string s = valueToString(a); // intelligent conversion
                return s;
            }
        }
        catch (...) {

        }
        return std::string("");
    }

    /**
     * getValue
     * @param path
     * @return
     */
    template <typename T, typename P>
    T getValue(P path) {
        try {
            ReadLock l(this->mutex());
            auto* n = this->root().find(path);
            if (n) {
                V& a = n->data();
                if (!a.empty()) {
                    return  valueToType<T>(a);
                }
            }
        }
        catch (...) {

        }
        return T();
    }

    /**
     * getValue
     * @param path
     * @return
     */
    template <typename T>
    T getValue(MRL::PropertyPath& p, const std::string& c) {
        if (!c.empty()) {
            try {
                ReadLock l(this->mutex());
                p.push_back(c);
                auto* n = this->root().find(p);
                p.pop_back();
                if (n) {
                    V& a = n->data();
                    if (!a.empty()) {
                        return  valueToType<T>(a);
                    }
                }
            }
            catch (...) {

            }
        }
        return T();
    }

    /**
     * MRL::VariantPropertyTree::sync
     * @param tree
     */
    void sync(ValueTree& /*tree*/) {

    }

    /**
     * printNode
     * @param os
     * @param n
     * @param level
     */
    void printNode(std::ostream& os, ValueNode* n, int level) {
        if (n) {
            std::string indent(level, ' ');
            os << indent << n->name() << " : " << valueToString(n->data()) << std::endl;
            if (n->children().size() > 0) {
                level++;
                for (auto i = n->children().begin(); i != n->children().end(); i++) {
                    printNode(os, i->second, level); // recurse
                }
            }
        }
    }

    // JSON

    /**
     * toJson
     * @param n
     * @param v
     */
    void toJson(ValueNode* n, Wt::Json::Object& v) {
        try {
            if (n) {
                // add value to object
                Wt::Json::Value ov(Wt::Json::ObjectType);
                Wt::Json::Object& o = ov;
                Wt::Json::Value to;
                setJson(to, n->data());
                o["value"] = to;

                if (n->children().size() > 0) {
                    Wt::Json::Value cv(Wt::Json::ObjectType);
                    Wt::Json::Object& c = cv; // set of children

                    // Now add children
                    for (auto i = n->children().begin(); i != n->children().end(); i++) {
                        toJson(i->second, c);
                    }

                    o["children"] = cv;
                }

                v[n->name()] = ov;  // add to parent
            }
        }
        catch (const std::exception& e) {
            EXCEPT_TRC
        }
        catch (...) {
            EXCEPT_DEF
        }
    }

    /**
     * fromJson
     * @param n
     * @param v
     */
    void fromJson(ValueNode* n, Wt::Json::Object& v) {
        try {
            if (n) {
                // Get the json object for this node
                if (v.contains(n->name())) {
                    Wt::Json::Object& no = v[n->name()]; // get the node object

                    if (no.contains("value")) {
                        getJson(no["value"], n->data());
                    }

                    if (no.contains("children")) {
                        // get the children
                        Wt::Json::Object& c = no["children"];
                        std::set< std::string > nnc = c.names();

                        // iterate the children - exception thrown on any inconsistency
                        for (auto i = nnc.begin(); i != nnc.end(); i++) {
                            ValueNode* ch = new ValueNode(*i, n);
                            fromJson(ch, c); // recurse
                            n->addChild(ch);
                        }
                    }
                }
            }
        }
        catch (const std::exception& e) {
            EXCEPT_TRC
        }
        catch (...) {
            EXCEPT_DEF
        }
    }

    /**
     * toJson
     * @param v
     */
    void toJson(Wt::Json::Object& v) {
        // whole tree
        toJson(this->rootNode(), v);
    }

    /**
     * fromJson
     * @param v
     */
    void fromJson(Wt::Json::Object& v) {
        // whole tree
        this->clear();
        fromJson(this->rootNode(), v);
    }

    /**
     * dump the property tree
     * @param os
     */
    void dump(std::ostream& os = std::cerr) {
        this->printNode(os, this->rootNode(), 0);
    }
};

} // namespace MRL

#endif // VALUETREE_H
