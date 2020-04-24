#include <iostream>
#include <open62541server.h>
#include "testmethod.h"
#include <serverrepeatedcallback.h>

namespace opc = Open62541;
using namespace std;

// example server with memory based historian

/**
 * The TestServer class
 */
class TestServer : public opc::Server {
    int m_idxNameSpace = 2;
    TestMethod _method;
    opc::NodeId eventType;

public:
    TestServer() {
    }

    void initialise(); // initialise the server before it runs but after it has been configured
};

/**
 * TestServer::initialise
 */
void TestServer::initialise() {
    cout << "initialise()" << endl;
    m_idxNameSpace = addNamespace("urn:test:test"); // create a name space

    cout << "Namespace " << m_idxNameSpace << endl;
    _method.initialise(*this);

    // Add a node and set its context to test context
    std::string nameFolder = "ServerMethodItem";
    opc::NodeId nodeFolder(m_idxNameSpace, nameFolder);

    if (!addFolder(opc::NodeId::Objects, nameFolder, nodeFolder, opc::NodeId::Null))
        return;

    std::string nameNumber = "Number_Value";
    opc::NodeId nodeNumber(m_idxNameSpace, nameNumber);
    opc::Variant valNumber(1);

    if (!addVariable(opc::NodeId::Objects, nameNumber, valNumber, nodeNumber, opc::NodeId::Null)) {
        cout << "Failed to create Node " << nameNumber << endl;
    }

    opc::NodeId methodId(m_idxNameSpace, "EventTrigger");
    if (_method.addServerMethod(*this, "TestEventTriggerMethod", nodeFolder, methodId, opc::NodeId::Null, m_idxNameSpace)) {
        cout << "Added TestMethod - Event Trigger Method - call from client (e.g. UAExpert)" << endl;
    }
    else {
        cout << "Failed to add method " << " " <<  UA_StatusCode_name(lastError()) << endl;
    }
}

/**
 * main
 * @return 
 */
int main(int/* argc*/, char **/*argv[]*/) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
