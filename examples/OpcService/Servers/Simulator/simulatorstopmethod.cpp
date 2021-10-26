#include "simulatorstopmethod.h"
#include "simulatoropc.h"

namespace opc = Open62541;

/**
 * SimulatorStopMethod::callback
 * @param server
 * @return UA_STATUSCODE_GOOD
 */
UA_StatusCode SimulatorStopMethod::callback(opc::Server &server,
                               const UA_NodeId* /*objectId*/,
                               size_t /*inputSize*/,
                               const UA_Variant * /*input*/,
                               size_t /*outputSize*/,
                               UA_Variant * /*output*/)
{
    _process.stop(); // stop the measuring process
    opc::Variant v("Stopped"); // set the status to stopped
    server.writeValue(_process.Status,v);
    return UA_STATUSCODE_GOOD;
}
