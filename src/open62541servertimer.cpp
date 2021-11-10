/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/open62541server.h>

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
    virtual ~Timer() { UA_Server_removeCallback(_server->server(), _id); }
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

//
typedef std::unique_ptr<Timer> TimerPtr;
std::map<UA_UInt64, TimerPtr> _timerMap;  // one map per client

void Server::timerCallback(UA_Server*, void* data)
{
    // timer callback
    if (data) {
        Timer* t = static_cast<Timer*>(data);
        if (t) {
            t->handle();
            if (t->oneShot()) {
                // Potential risk of the client disappearing
                t->server()->_timerMap.erase(t->id());
            }
        }
    }
}

/*!
 * \brief addTimedCallback
 * \param data
 * \param date
 * \param callbackId
 * \return
 */
bool Server::addTimedEvent(unsigned msDelay, UA_UInt64& callbackId, std::function<void(Timer&)> func)
{
    if (m_pServer) {
        UA_DateTime dt = UA_DateTime_nowMonotonic() + (UA_DATETIME_MSEC * msDelay);
        TimerPtr t(new Timer(this, 0, true, func));
        _lastError = UA_Server_addTimedCallback(m_pServer, Server::timerCallback, t.get(), dt, &callbackId);
        t->setId(callbackId);
        _timerMap[callbackId] = std::move(t);
        return lastOK();
    }
    callbackId = 0;
    return false;
}

/* Add a callback for cyclic repetition to the client.
 *
 * @param client The client object.
 * @param callback The callback that shall be added.
 * @param data Data that is forwarded to the callback.
 * @param interval_ms The callback shall be repeatedly executed with the given
 *        interval (in ms). The interval must be positive. The first execution
 *        occurs at now() + interval at the latest.
 * @param callbackId Set to the identifier of the repeated callback . This can
 *        be used to cancel the callback later on. If the pointer is null, the
 *        identifier is not set.
 * @return Upon success, UA_STATUSCODE_GOOD is returned. An error code
 *         otherwise. */

bool Server::addRepeatedTimerEvent(UA_Double interval_ms,
                                   UA_UInt64& callbackId,
                                   std::function<void(Timer&)> func)
{
    if (m_pServer) {
        TimerPtr t(new Timer(this, 0, false, func));
        _lastError = UA_Server_addRepeatedCallback(m_pServer, Server::timerCallback, t.get(), interval_ms, &callbackId);
        t->setId(callbackId);
        _timerMap[callbackId] = std::move(t);
        return lastOK();
    }
    callbackId = 0;
    return false;
}
/*!
 * \brief changeRepeatedCallbackInterval
 * \param callbackId
 * \param interval_ms
 * \return
 */
bool Server::changeRepeatedTimerInterval(UA_UInt64 callbackId, UA_Double interval_ms)
{
    if (m_pServer) {
        _lastError = UA_Server_changeRepeatedCallbackInterval(m_pServer, callbackId, interval_ms);
        return lastOK();
    }
    return false;
}
/*!
 * \brief UA_Client_removeCallback
 * \param client
 * \param callbackId
 */
void Server::removeTimerEvent(UA_UInt64 callbackId)
{
    _timerMap.erase(callbackId);
}

} // namespace Open62541
