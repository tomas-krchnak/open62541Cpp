#include "testmethod.h"

namespace opc = Open62541;

AdderMethod::AdderMethod() : opc::ServerMethod("Adder", 2, 1) {
    input1.setDataType(UA_TYPES_DOUBLE);
    input1.setDescription("First argument");
    input1.setName("Argument 1");
    input1.setValueRank(-1); /* scalar argument */

    input2.setDataType(UA_TYPES_DOUBLE);
    input2.setDescription("Second argument");
    input2.setName("Argument 2");
    input2.setValueRank(-1); /* scalar argument */

    in()[0] = input1.get();
    in()[1] = input2.get();

    output.setDataType(UA_TYPES_DOUBLE);
    output.setDescription("Output the sum of arguments 1 and 2");
    output.setName("Sum");
    output.setValueRank(-1);
    out()[0] = output.get();
}

// This method adds two numbers and returns the result
UA_StatusCode AdderMethod::callback(
    opc::Server&      /*server*/,
    const UA_NodeId*    objectId,
    size_t              inputSize,
    const UA_Variant*   input,
    size_t              outputSize,
    UA_Variant*         output) {

    // validate argument lists are the correct length
    if(inputSize == 2 && outputSize == 1) 
    {
        // assume double - but should validate
        UA_Double* arg1 = (UA_Double*)input[0].data;
        UA_Double* arg2 = (UA_Double*)input[1].data;
        double sum = *arg1 + *arg2;

        opc::Variant out_var(sum);
        out_var.assignTo(*output);
    }

    return UA_STATUSCODE_GOOD;
}
