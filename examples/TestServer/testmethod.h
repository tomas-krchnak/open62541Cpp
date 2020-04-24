#ifndef TESTMETHOD_H
#define TESTMETHOD_H

#include <servermethod.h>

namespace opc = Open62541;

class AdderMethod : public opc::ServerMethod
{
    opc::Argument input1; // argument definitions must persist
    opc::Argument input2; // argument definitions must persist
    opc::Argument output;

public:
    AdderMethod();

    /**
     * callback
     * @return 
     */
    virtual UA_StatusCode callback(
        opc::Server&        server,
        const UA_NodeId*    objectId,
        size_t              inputSize,
        const UA_Variant*   input,
        size_t              outputSize,
        UA_Variant*         output);
};

#endif // TESTMETHOD_H
