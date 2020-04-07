#include <iostream>
#include <open62541server.h>
#include "testcontext.h"
#include "testmethod.h"
#include <serverrepeatedcallback.h>
#include "testobject.h"

namespace opc = Open62541;
using namespace std;

// example server
class TestServer : public opc::Server {
    int                         _idx; // namespace index
    opc::SeverRepeatedCallback  _repeatedEvent;
    TestMethod                  _method;
    TestContext                 _context;
    TestObject                  _object;

public:
  TestServer()
    : _repeatedEvent(*this, 2000, [&](opc::SeverRepeatedCallback& s) {
        opc::NodeId nodeNumber(_idx, "Number_Value");
        int v = std::rand() % 100;
        opc::Variant numberValue(v);
        cout << "_repeatedEvent called setting number value = " << v << endl;
        s.server().writeValue(nodeNumber, numberValue);
      })
    , _object(*this)    {}

  void initialise() override; // initialise the server before it runs but after it has been configured
};

//*****************************************************************************

void TestServer::initialise() {
    _idx = addNamespace("urn:test:test"); // create a namespace

    // Add a node and set its context to test context
    opc::NodeId newFolder(_idx,"ServerMethodItem");

    if (!addFolder(opc::NodeId::Objects, "ServerMethodItem", newFolder, opc::NodeId::Null)) {
      cout << "Failed to add folder " << " " << UA_StatusCode_name(lastError()) << endl;
      return;
    }
    
    // Add a string value to the folder
    opc::NodeId variable(_idx, "String_Value");
    opc::Variant v("A String Value");

    if (!addVariable(newFolder, "String_Value", v, variable, opc::NodeId::Null, &_context)) {
        cout << "Failed to add node " << opc::toString(variable)
             << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
    // attach value callbacks to this node
    else if (!_context.setValueCallback(*this, variable)) {
        cout << "Failed to set value callback" << endl;
    }

    // Add a Number value to the folder
    cout << "Create Number_Value" << endl;
    opc::NodeId nodeNumber(_idx, "Number_Value");
    opc::Variant numberValue(1);
    if (!addVariable(opc::NodeId::Objects, "Number_Value", numberValue, nodeNumber, opc::NodeId::Null)) {
        cout << "Failed to create Number Value Node " << endl;
    }

    _repeatedEvent.start(); // Start repeated event

    // Create TestMethod node
    opc::NodeId methodId(_idx, 12345);
    if (_method.addServerMethod(*this, "TestMethod", newFolder, methodId, opc::NodeId::Null, _idx)) {
        cout << "Added TestMethod - Adds two numbers together - call from client (e.g. UAExpert)" << endl;
    }
    else {
        cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
    }

    // Define an object type
    opc::NodeId testType(_idx,"TestObjectType");
    if (_object.addType(testType)) {
        cout << "Added TestObject type" << endl;
    }
    else {
        cout << "Failed to create object type" << endl;
    }

    opc::NodeId exampleInstance(_idx,"ExampleInstance");
    _object.addInstance("ExampleInstance",newFolder,exampleInstance);
}

//*****************************************************************************

int main(int, char**) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
