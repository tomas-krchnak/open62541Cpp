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

#ifndef SERVERREPEATEDCALLBACK_H
#define SERVERREPEATEDCALLBACK_H

#include "open62541objects.h"

namespace Open62541 {

typedef std::function<void (SeverRepeatedCallback &)> SeverRepeatedCallbackFunc;

/**
 * The SeverRepeatedCallback class
 */
class UA_EXPORT SeverRepeatedCallback {
    Server&                     _server;    /**< parent server */
    UA_UInt32                   _interval   = 1000;
    UA_UInt64                   _id         = 0;
    SeverRepeatedCallbackFunc   _func;      /**< functor to handle event */

protected:
    UA_StatusCode               _lastError  = 0;

public:
    /**
     * callbackFunction
     * @param server
     * @param data
     */
    static void callbackFunction(UA_Server* server, void* data);

    /**
     * SeverRepeatedCallback 
     * @param server
     * @param interval
     */
    SeverRepeatedCallback(Server& server, UA_UInt32 interval)
        : _server(server)
        , _interval(interval) {}

    /**
    * SeverRepeatedCallback
    * This version takes a functor
    * @param s
    * @param interval
    * @param func
    */
    SeverRepeatedCallback(
        Server& server,
        UA_UInt32 interval,
        SeverRepeatedCallbackFunc func)
        : _server(server)
        , _interval(interval)
        , _func(func)           {}

    virtual ~SeverRepeatedCallback();

    /**
     * start
     * @return 
     */
    bool start();

    /**
     * changeInterval
     * @param interval
     * @return 
     */
    bool changeInterval(unsigned interval);

    /**
     * stop
     * @return true on success
     */
    bool stop();

    /**
     * lastOK
     * @return 
     */
    bool            lastOK()    const { return _lastError == UA_STATUSCODE_GOOD; }
    UA_StatusCode   lastError() const { return _lastError; }
    Server&         server()          { return _server; }
    UA_UInt64       id()        const { return _id; }

    /**
     * callback
     * if the functor is valid call it
     . no need to derive a handler class, unless you want to
     */
    virtual void callback()           { if (_func) _func(*this); }

};

/**
 * SeverRepeatedCallbackRef
 */
typedef std::shared_ptr<SeverRepeatedCallback> SeverRepeatedCallbackRef;

} // namespace Open62541

#endif // SERVERREPEATEDCALLBACK_H
