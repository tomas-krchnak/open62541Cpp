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

namespace Open62541 {

void SeverRepeatedCallback::callbackFunction(UA_Server* /*server*/, void* data) {
    if (auto p = (SeverRepeatedCallback*)data)
        p->callback();
}

//*****************************************************************************

bool SeverRepeatedCallback::start() {
    if (_id != 0 || !_server.server())
        return false;
    
    WriteLock l(_server.mutex());
    _lastError = UA_Server_addRepeatedCallback(
        _server.server(),
        callbackFunction,
        this,
        _interval,
        &_id);
    return lastOK();
}

//*****************************************************************************

bool SeverRepeatedCallback::changeInterval(unsigned interval) {
    if (_id == 0 || !_server.server())
        return false;
    
    WriteLock l(_server.mutex());
    _lastError = UA_Server_changeRepeatedCallbackInterval(
        _server.server(),
        _id,
        interval);
    return lastOK();
}

//*****************************************************************************

bool SeverRepeatedCallback::stop() {
    if (_id == 0)
        return false;
    
    if(_server.server()) {
        WriteLock l(_server.mutex());
        UA_Server_removeRepeatedCallback(_server.server(), _id);
        _id = 0;
        return true;
    }
    
    _id = 0;
    return false;
}

//*****************************************************************************

SeverRepeatedCallback::~SeverRepeatedCallback() {
    if(_server.server()) {
        WriteLock l(server().mutex());
        UA_Server_removeRepeatedCallback(_server.server(), _id);
    }
}

} // namespace Open62541
