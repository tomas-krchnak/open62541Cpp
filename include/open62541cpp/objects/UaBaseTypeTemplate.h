/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/

#ifndef UABASETYPETEMPLATE_H
#define UABASETYPETEMPLATE_H

#include <memory>
#include "open62541/types_generated.h"
#include "open62541/types.h"

#define UNKNOWN_UA_TYPE = -1000000

namespace Open62541 {
//
// Base wrapper for most C open62541 object types
// use unique_ptr
//
template <typename T, int TYPES_ARRAY_INDEX>
class UA_EXPORT TypeBase
{
    static_assert(TYPES_ARRAY_INDEX < UA_TYPES_COUNT, "TYPES_ARRAY_INDEX must be smaller than UA_TYPES_COUNT");

protected:
    struct Deleter {
        // Called by unique_ptr to destroy/free the Resource
        void operator()(T* r) { UA_delete(r, &UA_TYPES[TYPES_ARRAY_INDEX]); }
    };

    std::unique_ptr<T, Deleter> _d;  // shared pointer - there is no copy on change

private:
    void init()
    {
        if (_d) {
            clear();
        }
        _d = std::unique_ptr<T, Deleter>(static_cast<T*>(UA_new(&UA_TYPES[TYPES_ARRAY_INDEX])), Deleter());
    }

public:
    explicit TypeBase(T* t)
        : _d(t, Deleter())
    {
    }
    TypeBase()
        : _d(static_cast<T*>(UA_new(&UA_TYPES[TYPES_ARRAY_INDEX])), Deleter())
    {
        UA_init(_d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
    }

    virtual ~TypeBase() = default;

    T& get() const { return *(_d.get()); }
    // Reference and pointer for parameter passing
    operator T&() const { return get(); }

    operator T*() const { return _d.get(); }
    const T* constRef() const { return _d.get(); }

    T* ref() const { return _d.get(); }

    T* clearRef()
    {
        clear();
        return _d.get();
    }

    TypeBase(const T& t)
    {
        init();
        UA_copy(t, _d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
    }

    TypeBase(const TypeBase<T, TYPES_ARRAY_INDEX>& t)
    {
        init();
        UA_copy(t._d.get(), _d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
    }

    TypeBase<T, TYPES_ARRAY_INDEX>& operator=(const TypeBase<T, TYPES_ARRAY_INDEX>& t)
    {
        init();
        UA_copy(t._d.get(), _d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
        return *this;
    }

    TypeBase<T, TYPES_ARRAY_INDEX>& operator=(const T& t)
    {
        init();
        UA_copy(&t, _d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
        return *this;
    }

    void clear()
    {
        if (_d) {
            UA_clear(_d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
        }
    }

    void null()
    {
        clear();
        UA_init(_d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
    }

    void assignTo(T& v)
    {
        clear();
        UA_copy(_d.get(), &v, &UA_TYPES[TYPES_ARRAY_INDEX]);
    }
    void assignFrom(const T& v)
    {
        clear();
        UA_copy(&v, _d.get(), &UA_TYPES[TYPES_ARRAY_INDEX]);
    }
};

//
// Repeated for each type but cannot use C++ templates because we must also wrap the C function calls for each type
// initialisation implies shallow copy and so takes ownership, assignment is deep copy so source is not owned
//
// trace the object create and delete - only for detailed testing / debugging
#define UA_STRINGIFY(a) __UA_STRINGIFY(a)
#define __UA_STRINGIFY(a) #a

#ifdef UA_TRACE_OBJ
#define UA_TRC(s) std::cout << s << std::endl;
#else
#define UA_TRC(s)
#endif

    //
// copies are all deep copies
//
#define UA_TYPE_BASE(C, T)                                 \
    C()                                                    \
        : TypeBase(T##_new())                              \
    {                                                      \
        T##_init(_d.get());                                \
        UA_TRC("Construct:" << UA_STRINGIFY(C))            \
    }                                                      \
    C(const T& t)                                          \
        : TypeBase(T##_new())                              \
    {                                                      \
        assignFrom(t);                                     \
        UA_TRC("Construct (" << UA_STRINGIFY(T) << ")")    \
    }                                                      \
    ~C()                                                   \
    {                                                      \
        UA_TRC("Delete:" << UA_STRINGIFY(C));              \
        if (_d)                                            \
            T##_clear(_d.get());                           \
    }                                                      \
    C(const C& n)                                          \
        : TypeBase(T##_new())                              \
    {                                                      \
        T##_copy(n._d.get(), _d.get());                    \
        UA_TRC("Copy Construct:" << UA_STRINGIFY(C))       \
    }                                                      \
    C& operator=(const C& n)                               \
    {                                                      \
        UA_TRC("Assign:" << UA_STRINGIFY(C));              \
        null();                                            \
        T##_copy(n._d.get(), _d.get());                    \
        return *this;                                      \
    }                                                      \
    void null()                                            \
    {                                                      \
        if (_d) {                                          \
            UA_TRC("Delete(in null):" << UA_STRINGIFY(C)); \
            T##_clear(_d.get());                           \
        }                                                  \
        _d.reset(T##_new());                               \
        T##_init(_d.get());                                \
    }                                                      \
    void assignTo(T& v) { T##_copy(_d.get(), &v); }        \
    void assignFrom(const T& v) { T##_copy(&v, _d.get()); }

#define UA_TYPE_DEF(T) UA_TYPE_BASE(T, UA_##T)
}  // namespace Open62541


#endif /* UABASETYPETEMPLATE_H */
