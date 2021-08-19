#include <iostream>
#include <open62541cpp/open62541server.h>

namespace opc = Open62541;
using namespace std;

// example server with memory based historian

/**
 * The TestServer class
 */
class TestServer : public opc::Server {
    int _idx = 2; // namespace index

public:
    TestServer() {
        logins().resize(1);
        logins()[0].username = UA_STRING_STATIC("admin");
        logins()[0].password = UA_STRING_STATIC("password");
        enableSimpleLogin();

    }
    void initialise(); // initialise the server before it runs but after it has been configured
};

/**
 * TestServer::initialise
 */
void TestServer::initialise() {
    cout << "initialise()" << endl;
    _idx = addNamespace("urn:test:test"); // create a name space

    opc::NodeId nodeNumber(_idx, "Number_Value");
    opc::Variant numberValue(1);

    if (!addVariable(opc::NodeId::Objects, "Number_Value", numberValue, nodeNumber, opc::NodeId::Null)) {
        cout << "Failed to create Number Value Node " << endl;
    }

    // Set the user name and password
}

/**
 * main
 * @return 
 */
int main(int/* argc*/, char** /*argv[]*/) {
    TestServer server;
    cerr << "Starting server" << endl;
    server.start();
    cerr << "Server Finished" << endl;
    return 0;
}
