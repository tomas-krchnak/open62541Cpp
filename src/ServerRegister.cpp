#include <open62541cpp/ServerRegister.h>
#include <open62541cpp/open62541server.h>

namespace Open62541 {
std::map<const UA_Server*, Server*> ServerRegister::s_serverMap;

Server* ServerRegister::findServer(UA_Server* pUAServer)
{
    return s_serverMap[pUAServer];
}

Server* ServerRegister::findServer(const UA_Server* pUAServer)
{
    return s_serverMap[pUAServer];
}

void ServerRegister::addServer(const UA_Server* pUAServer, Server* pServer)
{
    s_serverMap[pUAServer] = pServer;
}
}  // namespace Open62541
