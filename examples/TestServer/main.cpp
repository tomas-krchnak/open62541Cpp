#include <iostream>
#include <open62541server.h>
#include <serverrepeatedcallback.h>
#include "testcontext.h"
#include "testmethod.h"
#include "testobject.h"

namespace opc = Open62541;
using namespace std;

// example server
class TestServer : public opc::Server {
    int                         m_idxNameSpace;
    opc::ServerRepeatedCallback m_CallBack_RollDice;
    AdderMethod                 m_Adder;
    TestContext                 m_context;
    TestObject                  m_object;
    const std::string           m_nameDice = "Dice result";

public:
  TestServer()
    : m_object(*this)
    , m_CallBack_RollDice(*this, 2000, [&](opc::ServerRepeatedCallback& s) {
        opc::NodeId nodeDice(m_idxNameSpace, m_nameDice);
        int v = std::rand() % 6;
        opc::Variant diceResult(v);
        cout << "New dice roll = " << v << endl;
        s.server().setValue(nodeDice, diceResult);
      }) {}

    /**
    * initialise the server before it runs but after it has been configured
    */
    void initialise() override;
};

//*****************************************************************************

void TestServer::initialise() {
    m_idxNameSpace = addNamespace("urn:test:test"); // create a namespace

    // Add a node and set its context to test context
    std::string nameFolder = "Methods";
    opc::NodeId nodeFolder(m_idxNameSpace, nameFolder);

    if (!addFolder(opc::NodeId::Objects, nameFolder, nodeFolder, opc::NodeId::Null)) {
      cout << "Failed to add folder " << nameFolder 
           << ": " << UA_StatusCode_name(lastError()) << endl;
      return;
    }
    
    // Add a string value to the folder
    std::string  nameManuf = "Manufacturer";
    opc::NodeId  nodeManuf(m_idxNameSpace, nameManuf);
    opc::Variant valuManuf("ThermoFisher");

    if (!addVariable(nodeFolder, nameManuf, valuManuf, nodeManuf, opc::NodeId::Null, &m_context)) {
        cout << "Failed to add node " << opc::toString(nodeManuf)
             << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
    // attach value callbacks to this node
    else if (!m_context.setValueCallback(*this, nodeManuf)) {
        cout << "Failed to set value callback" << endl;
    }

    // Add a Number value to the folder
    cout << "Create " << m_nameDice << endl;
    opc::NodeId nodeDice(m_idxNameSpace, m_nameDice);
    opc::Variant valDice(1);
    if (!addVariable(opc::NodeId::Objects, m_nameDice, valDice, nodeDice)) {
        cout << "Failed to create Node " << m_nameDice << endl;
    }

    m_CallBack_RollDice.start(); // Start repeated event

    // Create TestMethod node
    std::string methodName = m_Adder.name();
    opc::NodeId nodeAdder(m_idxNameSpace, 12345);
    if (m_Adder.addServerMethod(*this, methodName, nodeFolder, nodeAdder)) {
        cout << methodName << " method added - call from client (e.g. UAExpert)" << endl;
    }
    else {
        cout << "Failed to add " << methodName << " method. "
             <<  UA_StatusCode_name(lastError()) << endl;
    }

    // Define an object type
    opc::NodeId nodeDefinition(m_idxNameSpace, "TestObjectType");
    if (m_object.addType(nodeDefinition)) {
        cout << "TestObject type definition added." << endl;
    }
    else {
        cout << "Failed to create object type" << endl;
    }

    std::string nameInstance = "ExampleInstance";
    opc::NodeId exampleInstance(m_idxNameSpace, nameInstance);
    m_object.addInstance(nameInstance, nodeFolder, exampleInstance);
}

//*****************************************************************************

int main(int, char**) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
