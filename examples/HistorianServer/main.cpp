#include <iostream>
#include <open62541server.h>
#include "testcontext.h"
#include "testmethod.h"
#include <serverrepeatedcallback.h>
#include "testobject.h"
#include "historydatabase.h"

namespace opc = Open62541;
using namespace std;

//*****************************************************************************
// example server with memory based historian

class TestServer : public opc::Server {
    opc::MemoryHistorian        m_historian;     // the historian
    int                         m_idxNameSpace  = 2;
    opc::ServerRepeatedCallback m_repeatedEvent; // a periodic event - generates random number every 2 seconds
    const std::string           m_nameNumber    = "Number_Value";

public:
    TestServer()
        : m_repeatedEvent(*this, 2000, [&](opc::ServerRepeatedCallback& s) {
            opc::NodeId nodeNumber(m_idxNameSpace, m_nameNumber);
            int v = std::rand() % 100;
            opc::Variant numberValue(v);
            s.server().writeValue(nodeNumber, numberValue);
    }) {
        // Enable server as historian - must be done before starting server
        serverConfig().historyDatabase = m_historian.database();
        serverConfig().accessHistoryDataCapability = UA_TRUE;
    }

    void initialise() override; // initialise the server before it runs but after it has been configured
};

//*****************************************************************************

void TestServer::initialise() {
    cout << "initialise()" << endl;

    // create a name space
    m_idxNameSpace = addNamespace("urn:test:test");
    cout << "Namespace " << m_idxNameSpace << endl;;

    // Add a Historian node in test namespace
    cout << "Creating Historianised Node " << m_nameNumber << endl;
    opc::NodeId nodeNumber(m_idxNameSpace, m_nameNumber);
    opc::Variant valNumber(1);

    if (!addHistoricalVariable(opc::NodeId::Objects, m_nameNumber, valNumber, nodeNumber, opc::NodeId::Null)) {
        cout << "Failed to create Node " << m_nameNumber << endl;
    }
    else {
        m_historian.setUpdateNode(nodeNumber,*this); // adds the node the the historian - values are buffered as they are updated
    }

    // Start repeated event
    m_repeatedEvent.start();
}

//*****************************************************************************

int main(int /*argc*/, char** /*argv[]*/) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
