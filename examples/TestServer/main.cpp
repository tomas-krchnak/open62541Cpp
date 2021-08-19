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
class TestServer : public Server {
    int _idx; // namespace index
    TestMethod _method;
    TestContext _context;
    TestObject _object; // object
    NodeId _testTriggerSource;
    ServerMethod _eventMethod; // no arguments uses functor to handle
    NodeId _eventType;
    NodeId _eventNode;
public:
    TestServer() :  _object(*this),_eventMethod("EventTest",[this](Server &server,const UA_NodeId *, size_t, const UA_Variant *, size_t,UA_Variant *) {
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
    void initialise(); // initialise the server before it runs but after it has been configured
};

void TestServer::initialise() {
    _idx = addNamespace("urn:test:test"); // create a name space

    // Add the timers
    UA_UInt64 repeatedcallbackId = 0;
    addRepeatedTimerEvent(2000, repeatedcallbackId, [&](Server::Timer &s) {
        NodeId nodeNumber(_idx, "Number_Value");
        int v = std::rand() % 100;
        Variant numberValue(v);
        cout << "_repeatedEvent called setting number value = " << v <<  endl;
        s.server()->writeValue(nodeNumber,numberValue);
    });

    // Add one shot timer
    UA_UInt64 timedCallback = 0;
    addTimedEvent(5000,timedCallback,[&](Server::Timer &/*s*/) {
        cout << "Timed Event Triggered " << time(0) << endl ;
    });

    // Add a node and set its context to test context
    NodeId newFolder(_idx,"ServerMethodItem");
    if (addFolder(NodeId::Objects, "ServerMethodItem", newFolder,NodeId::Null)) {
        // Add a string value to the folder
        NodeId variable(_idx, "String_Value");
        Variant v("A String Value");
        if (!addVariable(NodeId::Objects, "String_Value", v, variable, NodeId::Null, &_context)) {
            cout << "Failed to add node " << toString(variable)
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
        if (!addVariable(NodeId::Objects, "TestTrigger", v, NodeId::Null, _testTriggerSource)) {
            cout << "Failed to add node " << toString(variable)
                 << " " <<  UA_StatusCode_name(lastError()) << endl;
        }


        cout << "Create Number_Value" << endl;
        NodeId nodeNumber(_idx, "Number_Value");
        Variant numberValue(1);
        if (!addVariable(NodeId::Objects, "Number_Value", numberValue, nodeNumber, NodeId::Null))
        {
            cout << "Failed to create Number Value Node " << endl;
        }
        //
        // Create TestMethod node
        //
        NodeId methodId(_idx, 12345);
        if (_method.addServerMethod(*this, "TestMethod", newFolder, methodId, NodeId::Null, _idx)) {
            cout << "Added TestMethod - Adds two numbers together - call from client (e.g. UAExpert)" << endl;
        }
        else {
            cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
        }
        //
        // Define an object type
        //
        NodeId testType(_idx,"TestObjectType");
        if(_object.addType(testType))
        {
            cout << "Added TestObject type" << endl;
        }
        else
        {
            cout << "Failed to create object type" << endl;
        }
        NodeId exampleInstance(_idx,"ExampleInstance");
        _object.addInstance("ExampleInstance",newFolder,exampleInstance);
        //
        // Add the event method
        //
        NodeId eventMethodId(_idx, 12346);
        if (_eventMethod.addServerMethod(*this, "EventMethod", newFolder, eventMethodId, NodeId::Null, _idx)) {
            cout << "Added EventMethod" << endl;
        }
        else {
            cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
        }


    }
    else {
        cout << "Failed to add folder " << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
}

int main(int/* argc*/, char** /*argv[]*/) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
