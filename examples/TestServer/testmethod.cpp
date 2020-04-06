#include "testmethod.h"

// This method adds two numbers and returns the result
UA_StatusCode TestMethod::callback(
    Open62541::Server&  /*server*/,
    const UA_NodeId*    objectId,
    size_t              inputSize,
    const UA_Variant*   input,
    size_t              outputSize,
    UA_Variant*         output) {

    // validate argument lists are the correct length
    if(inputSize == 2 && outputSize == 1) 
    {
        // assume double - but should validate
        UA_Double *arg1 = (UA_Double *)input[0].data;
        UA_Double *arg2 = (UA_Double *)input[1].data;
        double sum = *arg1 + *arg2;

        Open62541::Variant out_var(sum);
        out_var.assignTo(*output);
    }

    return UA_STATUSCODE_GOOD;
}
