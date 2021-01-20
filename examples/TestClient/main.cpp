#include <iostream>
#include <open62541client.h>

namespace opc = Open62541;
using namespace std;

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4850"

int main(int /*argc*/, char** /*argv*/) {
    cout << "Test Client" << endl;
    opc::Client client; // Construct client

    // Connect client to server
    if (!client.connect("opc.tcp://localhost:4840")) {
        cout << "Failed to connect" << endl;
        return 0;
    }

    int idx = client.namespaceGetIndex("urn:test:test");
    cout << "Get Endpoints" << endl;
    opc::EndpointDescriptionArray ea;
    client.getEndpoints("opc.tcp://localhost:4840", ea);
    
    for (size_t i = 0; i < ea.size(); i++) {
        cout << "End Point " << i << " = " << opc::toString(ea.at(i).endpointUrl) << endl;
    }

    // Browse for servers
    cout << "Create Path in Objects" << endl;

    opc::Path path = {"ClientDataFolder", "UnitA"};
    opc::NodeId unitAFolder;
    if (!client.createFolderPath(opc::NodeId::Objects, path, 1, unitAFolder.notNull())) {
        cout << "Failed to create folders" << endl;
        return 0;
    }
    
    cout << "Create Variable on Server" << endl;

    opc::NodeId variable(1, "A_Value");
    opc::Variant v(double(98.76));
    opc::NodeId newVariable;
    client.addVariable(unitAFolder, "A_Value", v, variable, newVariable.notNull());
    // Call Hello method
    cout << "Call TestHello method in server" << endl;
    opc::VariantList in;
    opc::VariantArray out;
    opc::NodeId MethodId(idx, 12345);

    opc::Variant arg0(1.25);
    opc::Variant arg1(3.8);
    in.push_back(arg0.get());
    in.push_back(arg1.get());

    opc::NodeId OwnerNode(idx, "ServerMethodItem");
    if (client.callMethod(OwnerNode, MethodId, in, out)) {
        if (out.size() > 0) {
            auto r = (UA_Double*)(out.data()[0].data);
            cout << "Result = " << *r << endl;
        }
    }
    else {
        UAPRINTLASTERROR(client.lastError());
    }

    // Discover servers
    cout << "Discovery of Servers" << endl;

    opc::StringArray serverUris;
    opc::StringArray localeIds;
    opc::ApplicationDescriptionArray registeredServers;
    opc::Client discoveryClient;

    if (!discoveryClient.findServers(DISCOVERY_SERVER_ENDPOINT, serverUris, localeIds, registeredServers)) {
        cout << "Failed to find discovery server" << endl;
        return 0;
    }
    
    cout << "Discovered Number of Servers: " << registeredServers.size() << endl;
    
    for (size_t i = 0; i < registeredServers.size(); i++) {
        UA_ApplicationDescription& description = registeredServers.at(i);
        cout << "Server [" << i << "]: " << description.applicationUri.length       << description.applicationUri.data       << endl;
        cout << "\n\tName [" << description.applicationName.text.length  << "] : "  << description.applicationName.text.data << endl;
        cout << "\n\tApplication URI: " << description.applicationUri.length        << description.applicationUri.data       << endl;
        cout << "\n\tProduct URI: "     << description.productUri.length << " "     <<  description.productUri.data          << endl;
        cout << "\n\tType: ";
        switch (description.applicationType) {
        case UA_APPLICATIONTYPE_SERVER:          cout << "Server"; break;
        case UA_APPLICATIONTYPE_CLIENT:          cout << "Client"; break;
        case UA_APPLICATIONTYPE_CLIENTANDSERVER: cout << "Client and Server"; break;
        case UA_APPLICATIONTYPE_DISCOVERYSERVER: cout << "Discovery Server";  break;
        default:                                 cout << "Unknown";
        }

        cout << endl << "\tDiscovery URLs:";

        for (size_t j = 0; j < description.discoveryUrlsSize; j++) {
            cout << endl << "\t\t" << j
                 << " " <<  description.discoveryUrls[j].length
                 << " " <<  description.discoveryUrls[j].data << endl;
        }

        cout << endl;
    }

    return 0;
}
