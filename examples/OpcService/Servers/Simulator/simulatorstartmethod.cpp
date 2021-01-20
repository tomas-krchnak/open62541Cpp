#include "simulatorstartmethod.h"
#include "simulatoropc.h"

namespace opc = Open62541;

/**
 * SimulatorStartMethod::callback
 * @return 
 */
UA_StatusCode SimulatorStartMethod::callback(opc::Server &server,
                               const UA_NodeId* /*objectId*/,
                               size_t /*inputSize*/,
                               const UA_Variant * /*input*/,
                               size_t /*outputSize*/,
                               UA_Variant * /*output*/)
{
    _process.start(); // stop the measuring process
    opc::Variant v("Ok"); // set the status to OK
    server.writeValue(_process.Status,v);
    return UA_STATUSCODE_GOOD;
}
