#include <iostream>
#include <open62541cpp/open62541server.h>
#include "testcontext.h"
#include "testmethod.h"
#include "testobject.h"

namespace opc = Open62541;
using namespace std;
//
// example server - this exercises timers as well
class EventContext : public NodeContext
{
public:
    EventContext() : NodeContext("Event") {}

};

//
class TestServer : public opc::Server {
    int _idx{}; // namespace index
    TestMethod _method;
    TestContext _context;
    TestObject _object; // object
    opc::NodeId _testTriggerSource;
    opc::ServerMethod _eventMethod; // no arguments uses functor to handle
    opc::NodeId _eventType;
    opc::NodeId _eventNode;
public:
    TestServer() :  _object(*this),_eventMethod("EventTest",[this](opc::Server &server,const UA_NodeId *, size_t, const UA_Variant *, size_t,UA_Variant *) {
        std::cerr << "Event Trigger Called " << std::endl;
        UA_ByteString bs;
        createEvent(_eventType,_eventNode);
        server.triggerEvent(_eventNode,_testTriggerSource,&bs,false);
        return UA_StatusCode(UA_STATUSCODE_GOOD);
    })
    {
        addNewEventType("SimpleEventType",_eventType,"The simple event type we created");
        _eventNode.notNull();
        setUpEvent(_eventNode,_eventType,"SimleEvent","TestServer");
    }
    void initialise() override; // initialise the server before it runs but after it has been configured
};

void TestServer::initialise() {
    _idx = addNamespace("urn:test:test"); // create a name space

    // Add the timers
    UA_UInt64 repeatedcallbackId = 0;
    addRepeatedTimerEvent(2000, repeatedcallbackId, [&](opc::Server::Timer &s) {
        opc::NodeId nodeNumber(_idx, "Number_Value");
        int v = std::rand() % 100;
        Variant numberValue(v);
        cout << "_repeatedEvent called setting number value = " << v <<  endl;
        s.server()->writeValue(nodeNumber,numberValue);
    });

    // Add one shot timer
    UA_UInt64 timedCallback = 0;
    addTimedEvent(5000,timedCallback,[&](opc::Server::Timer &/*s*/) {
        cout << "Timed Event Triggered " << time(0) << endl ;
    });

    // Add a node and set its context to test context
    opc::NodeId newFolder(_idx,"ServerMethodItem");
    if (!addFolder(opc::NodeId::Objects, "ServerMethodItem", newFolder, opc::NodeId::Null)) {
        cout << "Failed to add folder " << " " <<  UA_StatusCode_name(lastError()) << endl;
        return;
    }

    // Add a string value to the folder
    opc::NodeId variable(_idx, "String_Value");
    opc::Variant v("A String Value");
    if (!addVariable(opc::NodeId::Objects, "String_Value", v, variable, opc::NodeId::Null, &_context)) {
        cout << "Failed to add node " << opc::toString(variable)
             << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
    else {
        // attach value callbacks to this node
        if (!_context.setValueCallback(*this, variable)) {
            cout << "Failed to set value callback" << endl;
        }
    }

    // Set up an event source - monitor this item to get the events in UA Expert
    _testTriggerSource.notNull();
    if (!addVariable(opc::NodeId::Objects, "TestTrigger", v, opc::NodeId::Null, _testTriggerSource)) {
        cout << "Failed to add node " << opc::toString(variable)
             << " " <<  UA_StatusCode_name(lastError()) << endl;
    }


    cout << "Create Number_Value" << endl;
    opc::NodeId nodeNumber(_idx, "Number_Value");
    opc::Variant numberValue(1);
    if (!addVariable(opc::NodeId::Objects, "Number_Value", numberValue, nodeNumber, opc::NodeId::Null))
    {
        cout << "Failed to create Number Value Node " << endl;
    }
    //
    // Create TestMethod node
    //
    opc::NodeId methodId(_idx, 12345);
    if (_method.addServerMethod(*this, "TestMethod", newFolder, methodId, opc::NodeId::Null, _idx)) {
        cout << "Added TestMethod - Adds two numbers together - call from client (e.g. UAExpert)" << endl;
    }
    else {
        cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
    //
    // Define an object type
    //
    opc::NodeId testType(_idx,"TestObjectType");
    if(!_object.addType(testType))
    {
        cout << "Failed to create object type" << endl;
    }
    else
    {
        cout << "Added TestObject type" << endl;
    }

    opc::NodeId exampleInstance(_idx,"ExampleInstance");
    _object.addInstance("ExampleInstance",newFolder,exampleInstance);
    //
    // Add the event method
    //
    opc::NodeId eventMethodId(_idx, 12346);
    if (_eventMethod.addServerMethod(*this, "EventMethod", newFolder, eventMethodId, opc::NodeId::Null, _idx)) {
        cout << "Added EventMethod" << endl;
    }
    else {
        cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
    }

}

TestServer *server_instance = nullptr;
inline void StopHandler(int /*unused*/)
{
    if (server_instance)
        server_instance->stop();
    std::cout << "preparing to shut down..." << "\n";
}

inline void SetupSignalHandlers()
{
    signal(SIGINT, StopHandler);
    signal(SIGTERM, StopHandler);
}

int main(int/* argc*/, char** /*argv[]*/) {
    TestServer server;
    server_instance = &server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
