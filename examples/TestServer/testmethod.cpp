#include "testmethod.h"

namespace opc = Open62541;

AdderMethod::AdderMethod() : opc::ServerMethod("Adder", 2, 1) {
    input1.setDataType(UA_TYPES_DOUBLE)
          .setDescription("First argument")
          .setName("Argument 1")
          .setValueRank(-1); /* scalar argument */

    input2.setDataType(UA_TYPES_DOUBLE)
          .setDescription("Second argument")
          .setName("Argument 2")
          .setValueRank(-1); /* scalar argument */

    in()[0] = input1.get();
    in()[1] = input2.get();

    output.setDataType(UA_TYPES_DOUBLE)
          .setDescription("Output the sum of arguments 1 and 2")
          .setName("Sum")
          .setValueRank(-1);

    out()[0] = output.get();
}

// This method adds two numbers and returns the result
UA_StatusCode AdderMethod::callback(
    opc::Server&,         /*server*/
    const UA_NodeId*        objectId,
    const opc::VariantList& inputs,
          opc::VariantSpan& outputs) {

    // validate argument lists are the correct length
    if(inputs.size() != 2 || outputs.size() != 1)
        return UA_STATUSCODE_BADARGUMENTSMISSING;

    // assume double - but should validate
    auto* arg1 = (UA_Double*)inputs[0].data;
    auto* arg2 = (UA_Double*)inputs[1].data;
    double sum = *arg1 + *arg2;

    opc::Variant(sum).assignTo(*outputs.data());

    return UA_STATUSCODE_GOOD;
}
