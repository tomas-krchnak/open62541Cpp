#include <iostream>
#include <open62541server.h>
#include "testcontext.h"
#include "testmethod.h"
#include <serverrepeatedcallback.h>
#include "testobject.h"
#include "historydatabase.h"

namespace opc = Open62541;
using namespace std;

// example server with memory based historian

/******************************************************************************
 * The TestServer class
 */
class TestServer : public opc::Server {
    opc::MemoryHistorian _historian; // the historian
    int _idx = 2; // namespace index
    opc::SeverRepeatedCallback _repeatedEvent; // a periodic event - generates random number every 2 seconds
public:
    TestServer() :
        _repeatedEvent(*this, 2000, [ & ](opc::SeverRepeatedCallback & s) {
        opc::NodeId nodeNumber(_idx, "Number_Value");
        int v = std::rand() % 100;
        opc::Variant numberValue(v);
        s.server().writeValue(nodeNumber, numberValue);
    }) {
        // Enable server as historian - must be done before starting server
        serverConfig().historyDatabase = _historian.database();
        serverConfig().accessHistoryDataCapability = UA_TRUE;
    }

    void initialise(); // initialise the server before it runs but after it has been configured
};

/******************************************************************************
 * TestServer::initialise
 */
void TestServer::initialise() {
    cout << "initialise()" << endl;
    _idx = addNamespace("urn:test:test"); // create a name space

    cout << "Namespace " << _idx << endl;;
    // Add a node and set its context to test context
    cout << "Create Historianised Node Number_Value" << endl;

    opc::NodeId nodeNumber(_idx, "Number_Value");
    opc::Variant numberValue(1);

    if (!addHistoricalVariable(opc::NodeId::Objects, "Number_Value", numberValue, nodeNumber, opc::NodeId::Null)) {
        cout << "Failed to create Number Value Node " << endl;
    }
    else {
        _historian.setUpdateNode(nodeNumber,*this); // adds the node the the historian - values are buffered as they are updated
    }

    // Start repeated event
    _repeatedEvent.start();
}

/******************************************************************************
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
