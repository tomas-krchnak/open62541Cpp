#include <open62541server.h>
#include <open62541client.h>
#include <serverrepeatedcallback.h>

namespace opc = Open62541;
using namespace std;

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4850"

// This is an example server that registers with the discovery server
// give port and server name as arguments
class TestServer : public opc::Server {
    int                         m_idxNameSpace;
    UA_UInt64                   m_discoveryId;
    opc::SeverRepeatedCallback  m_CallBack_RollDice;
    opc::Client                 m_client;
    int                         m_res = 1;

public:
    TestServer(int port)
        : opc::Server(port)
        , m_CallBack_RollDice(*this, 2000, [&](opc::SeverRepeatedCallback & s) {
            opc::NodeId nodeDice(m_idxNameSpace, "Dice result");
            m_res = 1 + std::rand() % 6;
            opc::Variant numberValue(m_res);
            cout << "New dice roll = " << m_res << endl;
            s.server().writeValue(nodeDice, numberValue);
        }) {}

    /**
     * initialise the server before it runs but after it has been configured
     */
    void initialise();
};

//*****************************************************************************

void TestServer::initialise() {
    m_idxNameSpace = addNamespace("urn:test:test"); // create a name space

    // Add a node and set its context to test context
    std::string nameFolder = "Methods";
    opc::NodeId nodeFolder(m_idxNameSpace, nameFolder);
    if (!addFolder(opc::NodeId::Objects, nameFolder, nodeFolder))
        return;

    std::string nodeName = "Dice result";
    cout << "Create " << nodeName << endl;
    opc::NodeId nodeDice(m_idxNameSpace, nodeName);
    opc::Variant valDice(1);
    if (!addVariable(nodeFolder, nodeName, valDice, nodeDice)) {
        cout << "Failed to create Node " << nodeName << endl;
    }

    // Start repeated event - so it does something
    m_CallBack_RollDice.start();

    // connect to the discovery server
    if (!m_client.connect(DISCOVERY_SERVER_ENDPOINT)) {
        cerr << "Failed to connect with discovery server" << endl;
        return;
    }

    cerr << "Register with discovery server" << endl;
    static std::string endpoint(DISCOVERY_SERVER_ENDPOINT);

    if (!registerDiscovery(m_client)) {
        cerr << "Failed to register with discovery server" << endl;
        return;
    }

    cerr << "Registered with discovery server" << endl;
}

//*****************************************************************************

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: ServerDiscoverable <port> <Server Name>" << endl;
        return 0;
    }

    int port = ::atoi(argv[1]);
    std::string name(argv[2]);

    cerr << "Port: " << port << ", Name: " << name << endl;
    TestServer server(port);
    server.setMdnsServerName(name);
    server.setServerUri("Test Discoverable Server");
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;

    return 0;
}
