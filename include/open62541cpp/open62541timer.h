#ifndef OPEN62541TIMER_H
#define OPEN62541TIMER_H

/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/open62541objects.h>
#include <open62541cpp/objects/StringUtils.h>

namespace Open62541 {

    /*!
 * \brief The Timer class - used for timed events
 */
class Timer
{
    Server* _server = nullptr;
    UA_UInt64 _id   = 0;
    bool _oneShot   = false;
    std::function<void(Timer&)> _handler;

public:
    Timer() {}
    Timer(Server* c, UA_UInt64 i, bool os, std::function<void(Timer&)> func)
        : _server(c)
        , _id(i)
        , _oneShot(os)
        , _handler(func)
    {
    }
    virtual ~Timer();
    virtual void handle()
    {
        if (_handler)
            _handler(*this);
    }
    Server* server() const { return _server; }
    UA_UInt64 id() const { return _id; }
    void setId(UA_UInt64 i) { _id = i; }
    bool oneShot() const { return _oneShot; }
};

typedef std::unique_ptr<Timer> TimerPtr;

} // namespace Open62541
#endif
