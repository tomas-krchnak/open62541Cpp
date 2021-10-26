#ifndef SIMULATORSTARTMETHOD_H
#define SIMULATORSTARTMETHOD_H

#include <open62541cpp/servermethod.h>

namespace opc = Open62541;
class SimulateProcess;

class SimulatorStartMethod : public opc::ServerMethod
{
    opc::Argument inputArgument1; // argument definitions must persist
    opc::Argument outputArguments;
    SimulateProcess & _process;

public:
    SimulatorStartMethod(SimulateProcess &p) : opc::ServerMethod("Start",0,0),_process(p) {

    }

    virtual UA_StatusCode callback(
        opc::Server&  /*server*/,
        const UA_NodeId*    /*objectId*/,
        size_t              /*inputSize*/,
        const UA_Variant*   /*input*/,
        size_t              /*outputSize*/,
        UA_Variant*         /*output*/);
};

#endif // SIMULATORSTARTMETHOD_H
