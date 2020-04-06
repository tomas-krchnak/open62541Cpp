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
#include <serverrepeatedcallback.h>
#include <open62541server.h>


void Open62541::SeverRepeatedCallback::callbackFunction(UA_Server * /*server*/, void *data) {
    Open62541::SeverRepeatedCallback *p = (Open62541::SeverRepeatedCallback *)data;
    if (p) p->callback();
}

Open62541::SeverRepeatedCallback::SeverRepeatedCallback(Server &s, UA_UInt32 interval)
    : _server(s),
      _interval(interval) {
}

Open62541::SeverRepeatedCallback::SeverRepeatedCallback(Server &s, UA_UInt32 interval, SeverRepeatedCallbackFunc func)
    : _server(s),
      _interval(interval),
      _func(func) {
}

bool Open62541::SeverRepeatedCallback::start() {
    if ((_id == 0) && _server.server()) {
        WriteLock l(_server.mutex());
        _lastError = UA_Server_addRepeatedCallback(_server.server(), callbackFunction, this, _interval, &_id);
        return lastOK();
    }
    return false;
}

bool Open62541::SeverRepeatedCallback::changeInterval(unsigned i) {
    if ((_id != 0) && _server.server()) {
        WriteLock l(_server.mutex());
        _lastError = UA_Server_changeRepeatedCallbackInterval(_server.server(), _id, i);
        return lastOK();
    }
    return false;
}

bool Open62541::SeverRepeatedCallback::stop() {
    if (_id != 0) {
        if(_server.server())
        {
            WriteLock l(_server.mutex());
            UA_Server_removeRepeatedCallback(_server.server(), _id);
            _id = 0;
            return true;
        }
    }
    _id = 0;
    return false;
}

Open62541::SeverRepeatedCallback::~SeverRepeatedCallback() {
    if(_server.server())
    {
        WriteLock l(server().mutex());
        UA_Server_removeRepeatedCallback(_server.server(), _id);
    }
}


