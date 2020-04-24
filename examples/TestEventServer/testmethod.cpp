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

    if (!server.setUpEvent(eventNodeId, m_eventTypeTest, "TestEvent", "TestEventServer")) {
        std::cout << "Failed to create event" << UA_StatusCode_name(server.lastError()) << std::endl;
        return UA_STATUSCODE_GOOD; // ??? why good
    }
    if (!server.triggerEvent(eventNodeId)) {
        std::cout << "Failed to trigger event" << UA_StatusCode_name(server.lastError()) << std::endl;
        return UA_STATUSCODE_GOOD; // ??? why good
    }
    std::cout << "Event Triggered" << std::endl;
    return UA_STATUSCODE_GOOD;
}

//*****************************************************************************

bool TestMethod::initialise(opc::Server &server)
{
   m_eventTypeTest.notNull();

   if (server.addNewEventType("TestEvent", m_eventTypeTest, "Example Event")) {
       std::cout << "Added Event Type Event Node " << opc::toString(m_eventTypeTest) << std::endl;
       return true;
   }

    std::cout << "Failed to add type " << UA_StatusCode_name(server.lastError()) << std::endl;
    return false;
}

