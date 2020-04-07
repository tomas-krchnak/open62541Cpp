#ifndef VARIANT_H
#define VARIANT_H
#include "propertytree.h"
#include <map>
#include <typeinfo>
#include <string>
#include <functional>
#include <memory>
#include <boost/variant.hpp>
#include <list>
#include <Wt/Json/Value> // JSON support

namespace MRL {

typedef boost::variant<int, unsigned, double, std::string, bool, time_t, void *> Variant;
typedef std::list<Variant> VariantList; /**< list of variants */
typedef boost::shared_ptr<VariantList> VariantListPtr;
typedef std::vector<std::string> StringList;
typedef std::map<std::string,Variant> VariantMap;
typedef boost::shared_ptr<VariantMap> VariantMapPtr;

/**
 * Return the string representation of a Variant
 * @param v is the Variant to convert
 * @return a std::string
 */
std::string toString(const Variant &v);

/**
 * Return the string representation of a Variant for JSON
 * Same as toString except for std::string wich are enclosed in ""
 * @param v is the Variant to convert
 * @return a std::string
 */
std::string toJsonString(const Variant &v);

/**
 * PropertyPath
 */
typedef NodePath<std::string> PropertyPath;

// convert to/from JSON
void setJson(Wt::Json::Value &, Variant &);
void getJson(Wt::Json::Value &, Variant &);

/**
  * isType
  * @param a
  * @return 
  */
template <typename T>
inline bool isType(Variant &a) {
    return a.type().hash_code() == typeid(T).hash_code();
}

inline const std::string valueToString(const MRL::Variant &v) {
    return toString(v);
}

template <typename T> const T &valueToType(const MRL::Variant &v) {
    return boost::get<T>(v);
}

} // namespace MRL

#endif // VARIANT_H
