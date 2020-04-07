#include "testmethod.h"
#include <iostream>
#include <open62541server.h>

namespace opc = Open62541;

UA_StatusCode TestMethod::callback(
    opc::Server&          server,
    const UA_NodeId*    /*objectId*/,
    size_t              /*inputSize*/,
    const UA_Variant*   /*input*/,
    size_t              /*outputSize*/,
    UA_Variant*         /*output*/) {

    /* set up event */
    opc::NodeId eventNodeId;
    if (server.setUpEvent(eventNodeId, eventType, "TestEvent", "TestEventServer")) {
        if (server.triggerEvent(eventNodeId)) {
            std::cout << "Event Triggered" << std::endl;
        }
        else {
            std::cout << "Failed to trigger event" << UA_StatusCode_name(server.lastError()) << std::endl;
        }
    }
    else {
        std::cout << "Failed to create event" << UA_StatusCode_name(server.lastError()) << std::endl;
    }

    return UA_STATUSCODE_GOOD;
}


bool TestMethod::initialise(opc::Server &server)
{
   eventType.notNull();
   if (server.addNewEventType("TestEvent", eventType, "Example Event")) {
       std::cout << "Added Event Type Event Node " << opc::toString(eventType) << std::endl;
   }
   else {
       std::cout << "Failed to add type " << UA_StatusCode_name(server.lastError()) << std::endl;
   }
}

