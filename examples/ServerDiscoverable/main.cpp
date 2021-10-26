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
    opc::ServerRepeatedCallback m_CallBack_RollDice;
    opc::Client                 m_client;
    int                         m_res = 1;

public:
    TestServer(int port)
        : opc::Server(port)
        {}

    /**
     * initialise the server before it runs but after it has been configured
     */
    void initialise();
};

//*****************************************************************************

void TestServer::initialise() {
    m_idxNameSpace = addNamespace("urn:test:test"); // create a name space
    UA_UInt64 repeatedcallbackId = 0;
    addRepeatedTimerEvent(2000, repeatedcallbackId, [&](Server::Timer& s) {
        NodeId nodeNumber(_idx, "Number_Value");
        int v = std::rand() % 100;
        Variant numberValue(v);
        cout << "_repeatedEvent called setting number value = " << v << endl;
        s.server()->writeValue(nodeNumber, numberValue);
        });

    // Add one shot timer
    UA_UInt64 timedCallback = 0;
    addTimedEvent(5000, timedCallback, [&](Server::Timer&/*s*/) {
        cout << "Timed Event Triggered " << time(0) << endl;
        });
    // Add a node and set its context to test context
    NodeId newFolder(_idx, "ServerItems");
    if (addFolder(NodeId::Objects, "ServerItems", newFolder, NodeId::Null)) {
        cout << "Create Number_Value" << endl;
        NodeId nodeNumber(_idx, "Number_Value");
        Variant numberValue(1);
        if (!addVariable(NodeId::Objects, "Number_Value", numberValue, nodeNumber, NodeId::Null)) {
            cout << "Failed to create Number Value Node " << endl;
        }
        //
        // Start repeated event - so it does something
        // connect to the discovery server
        if (_client.connect(DISCOVERY_SERVER_ENDPOINT)) {
            cerr << "Register with discovery server" << endl;
            if (!registerDiscovery( _client)) {
                cerr << "Failed to register with discovery server" << endl;
            }
            else {
                cerr << "Registered with discovery server" << endl;
            }
        }
        else
        {
            cerr << "Failed to connect with discovery server" << endl;
        }

    }
}
/*!
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
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
