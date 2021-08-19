#ifndef TESTMETHOD_H
#define TESTMETHOD_H

#include <open62541cpp/servermethod.h>

namespace opc = Open62541;

class TestMethod : public opc::ServerMethod
{
    opc::NodeId m_eventTypeTest;

public:
    TestMethod() : opc::ServerMethod("TriggerEvent", 0, 0) {
    }

    bool initialise(opc::Server &server) override; // ServerMethod

    /**
     * callback
     * @return 
     */
    UA_StatusCode callback(
        opc::Server&        /*server*/,
        const UA_NodeId*    /*objectId*/,
        size_t              /*inputSize*/,
        const UA_Variant*   /*input*/,
        size_t              /*outputSize*/,
        UA_Variant*         /*output*/) override; // ServerMethod
};

#endif // TESTMETHOD_H
