#ifndef TESTMETHOD_H
#define TESTMETHOD_H

#include <open62541cpp/servermethod.h>

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
    UA_StatusCode callback(
        opc::Server&            server,
        const UA_NodeId*        objectId,
        const opc::VariantList& inputs,
              opc::VariantSpan& outputs) override; // opc::ServerMethod
};

#endif  // TESTMETHOD_H
