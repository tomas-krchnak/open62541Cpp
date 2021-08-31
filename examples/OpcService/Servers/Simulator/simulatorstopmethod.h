#ifndef SIMULATORSTOPMETHOD_H
#define SIMULATORSTOPMETHOD_H

#include <open62541cpp/servermethod.h>

namespace opc = Open62541;
class SimulateProcess;

class SimulatorStopMethod : public opc::ServerMethod
{
    opc::Argument inputArgument1; // argument definitions must persist
    opc::Argument outputArguments;
    SimulateProcess & _process;

public:
    SimulatorStopMethod(SimulateProcess &p) : opc::ServerMethod("Stop",0,0),_process(p) {

    }

    virtual UA_StatusCode callback(
        opc::Server&        /*server*/,
        const UA_NodeId*    /*objectId*/,
        size_t              /*inputSize*/,
        const UA_Variant*   /*input*/,
        size_t              /*outputSize*/,
        UA_Variant*         /*output*/);
};

#endif  // SIMULATORSTOPMETHOD_H
