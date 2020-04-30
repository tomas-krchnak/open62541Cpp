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
    Server&                     _server;            /**< parent server */
    UA_UInt32                   _interval   = 1000;
    UA_UInt64                   _id         = 0;    /**< call-back id on the server once created. */
    SeverRepeatedCallbackFunc   _func;              /**< functor to handle event */

protected:
    UA_StatusCode               _lastError  = UA_STATUSCODE_GOOD; /**< error code of the last called UA function. */

public:
    /**
     * callbackFunction
     * @param server (unused) specify the server
     * @param func the callback to call.
     */
    static void callbackFunction(UA_Server* server, void* func);

    /**
     * Constructor with empty call-back.
     * It isn't added to the server, just initialized.
     * @param server the server of the call-back.
     * @param interval specify the duration between each calls in ms.
     */
    SeverRepeatedCallback(Server& server, UA_UInt32 interval)
        : _server(server)
        , _interval(interval) {}

    /**
    * Constructor with call-back.
     * It isn't added to the server, just initialized.
     * @param server the server of the call-back.
     * @param interval specify the duration between each calls in ms.
    * @param func function of the call-back.
    */
    SeverRepeatedCallback(
        Server&                     server,
        UA_UInt32                   interval,
        SeverRepeatedCallbackFunc   func)
        : _server(server)
        , _interval(interval)
        , _func(func)           {}

    /**
    * Destructor. Remove the repeated call-back from the server, thread-safely.
    */
    virtual ~SeverRepeatedCallback();

    /**
     * Add the call-back for cyclic repetition to the server, thread-safely.
     * Repetition starts immediately.
     * The first execution occurs at now() + interval at the latest.
     * Also stores the attributed id of the repeated callback. Will be used to stop it.
     * @return true on success.
     */
    bool start();

    /**
     * Change the interval duration after repetition has started.
     * @param interval specify the new duration between each calls in ms.
     * @return true on success.
     */
    bool changeInterval(unsigned interval);

    /**
     * Stop the call-back repetition. Removes the call-back from the server, thread-safely.
     * @return true if the repetition was running. False if it was never started or already stopped.
     */
    bool stop();

    bool            lastOK()    const { return _lastError == UA_STATUSCODE_GOOD; }
    UA_StatusCode   lastError() const { return _lastError; }
    Server&         server()          { return _server; }
    UA_UInt64       id()        const { return _id; }

    /**
     * Call the call-back function.
     * if the functor is valid call it.
     * No need to derive a handler class, unless you want to.
     */
    virtual void callback()           { if (_func) _func(*this); }

};

/**
 * SeverRepeatedCallbackRef
 */
typedef std::shared_ptr<SeverRepeatedCallback> SeverRepeatedCallbackRef;

} // namespace Open62541

#endif // SERVERREPEATEDCALLBACK_H
