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

ServerRepeatedCallback::~ServerRepeatedCallback() {
    if(m_server.server()) {
        WriteLock l(server().mutex());
        UA_Server_removeRepeatedCallback(m_server.server(), m_id);
    }
}

//*****************************************************************************

void ServerRepeatedCallback::callbackFunction(UA_Server* /*server*/, void* pCallBack) {
    if (auto p = (ServerRepeatedCallback*)pCallBack)
        p->callback();
}

//*****************************************************************************

bool ServerRepeatedCallback::start() {
    if (m_id != 0 || !m_server.server())
        return false;
    
    WriteLock l(m_server.mutex());
    m_lastError = UA_Server_addRepeatedCallback(
        m_server.server(),
        callbackFunction,
        this,
        m_interval,
        &m_id);
    return lastOK();
}

//*****************************************************************************

bool ServerRepeatedCallback::changeInterval(unsigned interval) {
    if (m_id == 0 || !m_server.server())
        return false;
    
    WriteLock l(m_server.mutex());
    m_lastError = UA_Server_changeRepeatedCallbackInterval(
        m_server.server(),
        m_id,
        interval);
    return lastOK();
}

//*****************************************************************************

bool ServerRepeatedCallback::stop() {
    if (m_id == 0 || !m_server.server()) {
        m_id = 0;
        return false;
    }
    
    WriteLock l(m_server.mutex());
    UA_Server_removeRepeatedCallback(m_server.server(), m_id);
    m_id = 0;
    return true;
}

} // namespace Open62541
