#ifndef SERVERREGISTER_H
#define SERVERREGISTER_H

#include <open62541cpp/open62541objects.h>
#include <map>

namespace Open62541 {

class ServerRegister
{
    static std::map<const UA_Server*, Server*> s_serverMap;// Map UA_Server pointer key to servers pointer value

public:
    /**
     * Find an existing Server by its UA_Server pointer.
     * Used by call-backs to verify the server exists and is still running.
     * @param pUAServer a pointer on the Server underlying UA_Server.
     * @return a pointer on the matching Server
     */
    static Server* findServer(UA_Server* pUAServer);
    static Server* findServer(const UA_Server* pUAServer);
    static void addServer(const UA_Server* pUAServer, Server* pServer);
};


}// namespace open62541
#endif //SERVERREGISTER_H
