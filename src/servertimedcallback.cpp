#include <open62541cpp/servertimedcallback.h>
#include <open62541cpp/open62541server.h>

namespace Open62541 {

//
std::set<ServerTimedCallback*> ServerTimedCallback::_map;
//

/*!

    \brief ServerTimedCallback::callbackFunction
    \param server
    \param data
*/
void ServerTimedCallback::callbackFunction(UA_Server* /*server*/, void* data)
{
    ServerTimedCallback* p = (ServerTimedCallback*)data;
    if (p) {
        p->callback();
    }
}

/*!
    \brief ServerTimedCallback::ServerTimedCallback
    \param s
    \param interval
*/
ServerTimedCallback::ServerTimedCallback(Server& s, unsigned delay)
    : _server(s)
    , _interval(UA_DateTime_nowMonotonic() + delay)
{
    _map.insert(this);
}

/*!
    \brief ServerTimedCallback
    This version takes a functor
    \param s
    \param interval
    \param func
*/
ServerTimedCallback::ServerTimedCallback(Server& s, ServerTimedCallbackFunc func, unsigned delay)
    : _server(s)
    , _interval(UA_DateTime_nowMonotonic() + delay)
    , _func(func)
{
    _map.insert(this);
}

/*!
 * \brief ServerRepeatedCallback::stop
 * \return
 */
bool ServerTimedCallback::stop()
{
    if (_id != 0) {
        if (_server.server()) {
            WriteLock l(_server.mutex());
            UA_Server_removeRepeatedCallback(_server.server(), _id);  // possible to remove
            _id = 0;
            return true;
        }
    }
    _id = 0;
    return false;
}

/*!
    \brief ServerTimedCallback::start
    \return
*/
bool ServerTimedCallback::start()
{
    if ((_id == 0) && _server.server()) {
        WriteLock l(_server.mutex());
        _lastError =
            UA_Server_addTimedCallback(_server.server(), ServerTimedCallback::callbackFunction, this, _interval, &_id);
        return lastOK();
    }
    return false;
}

//
//
/*!
    \brief ServerTimedCallback::~ServerTimedCallback
*/
ServerTimedCallback::~ServerTimedCallback()
{
    stop();
    auto i = _map.find(this);
    if (i != _map.end())
        _map.erase(i);
}
}  // namespace Open62541
