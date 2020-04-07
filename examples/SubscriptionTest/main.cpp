#include <iostream>

#include "open62541client.h"
#include "clientsubscription.h"
#include "monitoreditem.h"

namespace opc = Open62541;
using namespace std;

class SubTestClient : public opc::Client
{
public:
    void asyncService(
        void*   /*userdata*/,
        UA_UInt32 requestId,
        void*   /*response*/,
        const UA_DataType* responseType) {
        cout << "asyncService requerstId = " << requestId << " Type " << responseType->typeName << endl;
    }

    void asyncConnectService(
        UA_UInt32 requestId,
        void*   /*userData*/,
        void*   /*response*/) {
        cout << "asyncConnectService requestId = " << requestId  << endl;
    }
};

//*****************************************************************************

int main() {
    cout << "Client Subscription Test - TestServer must be running" << endl;

    // Test subscription create
    // Monitored Items
    // Events
    opc::Client client;
    if (!client.connect("opc.tcp://localhost:4840")) {
        cout << "Subscription Failed" << endl;
        return 0;
    }
    
    int idx = client.namespaceGetIndex("urn:test:test");
    if (idx < 2) {
        cout << "TestServer not running idx = " << idx << endl;
        return 0;
    }
    
    cout << "Connected" << endl;
    UA_UInt32 subId = 0;

    if (!client.addSubscription(subId)) {
        cout << "Subscription Failed" << endl;
        return 0;
    }

    cout << "Subscription Created id = " << subId << endl;

    auto f = [](opc::ClientSubscription& c, UA_DataValue* v) {
        cout << "Data Change SubId " << c.id()
             << " Value " << v->value.type->typeName
             << " " << opc::dataValueToString(v) << endl;
    };

    auto ef = [](opc::ClientSubscription& c, opc::VariantArray&) {
        cout << "Event SubId " << c.id()  << endl;
    };

    cout << "Adding a data change monitor item" << endl;

    opc::NodeId nodeNumber(idx, "Number_Value");
    opc::ClientSubscription& cs = *client.subscription(subId);
    unsigned mdc = cs.addMonitorNodeId(f, nodeNumber); // returns monitor id
    if (!mdc) {
        cout << "Failed to add monitor data change" << endl;
    }

    cout << "Monitor events" << endl;

    // Set up the SELECT clauses
    auto efs = new opc::EventFilterSelect(2); // two select clauses
    efs->selectClause().setBrowsePath(0, "Message");
    efs->selectClause().setBrowsePath(1, "Severity");

    opc::NodeId en(0, 2253); // Root->Objects->Server

    // returns monitor id - ownership is transfered to monitoring item
    unsigned mev = cs.addEventMonitor(ef, en, efs);
    if (!mev) {
        cout << "Failed to monitor events" << endl;
    }

    // run for one minute
    for (int j = 0; j < 60; j++) {
        client.runIterate(1000);
    }

    cout << "Ended Run - Test if deletes work correctly" << endl;
    client.subscriptions().clear();
    cout << "Subscriptions cleared - run for another 5 seconds" << endl;

    for (int j = 0; j < 5; j++) {
        client.runIterate(1000);
    }

    cout << "Finished" << endl;

    return 0;
}
