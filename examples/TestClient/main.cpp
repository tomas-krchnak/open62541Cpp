#include <iostream>
#include <open62541client.h>

namespace opc = Open62541;
using namespace std;

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4850"

int main(int /*argc*/, char** /*argv*/) {
    cout << "Test Client" << endl;
    //
    // Construct client
    opc::Client client;
    // Connect
    if (client.connect("opc.tcp://localhost:4840")) {
        //

        int idx = client.namespaceGetIndex("urn:test:test");
        cout << "Get Endpoints" << endl;
        opc::EndpointDescriptionArray ea;
        client.getEndpoints("opc.tcp://localhost:4840", ea);
        //
        for (size_t i = 0; i < ea.length(); i++) {
            cout << "End Point " << i << " = " << opc::toString(ea.at(i).endpointUrl) << endl;
        }
        //
        // Browse for servers

        //
        cout << "Create Path in Objects" << endl;
        //
        opc::Path path = {"ClientDataFolder", "UnitA"};
        opc::NodeId unitAFolder;
        if (client.createFolderPath(opc::NodeId::Objects, path, 1, unitAFolder.notNull())) {
            cout << "Create Variable on Server" << endl;
            //
            opc::NodeId variable(1, "A_Value");
            opc::Variant v(double(98.76));
            opc::NodeId newVariable;
            client.addVariable(unitAFolder, "A_Value", v, variable, newVariable.notNull());
            // Call Hello method
            cout << "Call TestHello method in server" << endl;
            opc::VariantList in;
            opc::VariantCallResult out;
            opc::NodeId MethodId(idx, 12345);
            //
            opc::Variant arg0(1.25);
            opc::Variant arg1(3.8);
            in.push_back(arg0.get());
            in.push_back(arg1.get());

            //
            opc::NodeId OwnerNode(idx, "ServerMethodItem");
            if (client.callMethod(OwnerNode, MethodId, in, out)) {
                if (out.size() > 0) {
                    UA_Double *r = (UA_Double *)(out.data()[0].data);
                    cout << "Result = " << *r << endl;
                }
            }
            else {
                UAPRINTLASTERROR(client.lastError());
            }
            //
            // Discover servers
            cout << "Discovery of Servers" << endl;
            //
            opc::StringArray serverUris;
            opc::StringArray localeIds;
            opc::ApplicationDescriptionArray registeredServers;
            opc::Client discoveryClient;
            discoveryClient.initialise();
            //
            if (discoveryClient.findServers(DISCOVERY_SERVER_ENDPOINT, serverUris, localeIds, registeredServers)) {
                cout << "Discovered Number of Servers: " << registeredServers.length() << endl;
                for (size_t i = 0; i < registeredServers.length(); i++) {

                    UA_ApplicationDescription &description = registeredServers.at(i);
                    cout << "Server [" << i << "]: " << description.applicationUri.length  << description.applicationUri.data << endl;
                    cout << "\n\tName [" << description.applicationName.text.length << "] : " << description.applicationName.text.data << endl;
                    cout << "\n\tApplication URI: " << description.applicationUri.length << description.applicationUri.data << endl;
                    cout << "\n\tProduct URI: " <<   description.productUri.length << " " <<  description.productUri.data << endl;
                    cout << "\n\tType: ";
                    switch (description.applicationType) {
                        case UA_APPLICATIONTYPE_SERVER:
                            cout << "Server";
                            break;
                        case UA_APPLICATIONTYPE_CLIENT:
                            cout << "Client";
                            break;
                        case UA_APPLICATIONTYPE_CLIENTANDSERVER:
                            cout << "Client and Server";
                            break;
                        case UA_APPLICATIONTYPE_DISCOVERYSERVER:
                            cout << "Discovery Server";
                            break;
                        default:
                            cout << "Unknown";
                    }
                    cout << endl <<  "\tDiscovery URLs:";
                    for (size_t j = 0; j < description.discoveryUrlsSize; j++) {
                        cout << endl << "\t\t" << j  << " " <<  description.discoveryUrls[j].length
                             << " " <<  description.discoveryUrls[j].data << endl;
                    }
                    cout << endl;
                }

            }
            else {
                cout << "Failed to find discovery server" << endl;
            }


            cout << "Test Timers" << endl;

            // Now run the timer tests
            // set up timed event for 10 seconds time
            UA_UInt64 callerId;
            client.addTimedEvent(10000, callerId,[](opc::Client::Timer &){ std::cerr << "Timed Event Triggered " << time(0) << std::endl;});
            std::cerr << "Added one shot timer event for 10 seconds time  Now = " << time(0)  << " Id = " << callerId << endl;
            //
            // Add a repeated timer event - these can be thought of as event driven tasks
            //
            client.addRepeatedTimerEvent(2000,callerId,[](opc::Client::Timer &){ std::cerr << "Repeated Event Triggered " << time(0) << std::endl;});
            std::cerr << "Added repeated timer event for 2 seconds  Now = " << time(0)  << " Id = " << callerId << endl;
            //
            client.run(); // this will loop until interrupted


        }
        else {
            cout << "Failed to create folders" << endl;
        }
    }
    else {
        cout << "Failed to connect" << endl;
    }
    return 0;
}
