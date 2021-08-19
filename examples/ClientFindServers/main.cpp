#include <iostream>
#include <open62541cpp/open62541client.h>
#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4850"

namespace opc = Open62541;
using namespace std;

int main() {
    opc::Client client;
    // Connect client to server
    if (!client.connect("opc.tcp://localhost:4850"))
        return 0;

    cout << "Discovery of Servers" << endl;

    opc::StringArray serverUris;
    opc::StringArray localeIds;
    opc::ApplicationDescriptionArray registeredServers;

    if (!client.findServers(DISCOVERY_SERVER_ENDPOINT, serverUris, localeIds, registeredServers))
        return 0;
    
    cout << "Discovered Number of Servers: " << registeredServers.size() << endl;
    for (size_t i = 0; i < registeredServers.size(); i++) {
        UA_ApplicationDescription &description = registeredServers.at(i);
        cout << "Server [" << i << "]: " << description.applicationUri.length  << description.applicationUri.data << endl;
        cout << "\n\tName [" << description.applicationName.text.length << "] : " << description.applicationName.text.data << endl;
        cout << "\n\tApplication URI: " << description.applicationUri.length << description.applicationUri.data << endl;
        cout << "\n\tProduct URI: " <<   description.productUri.length << " " <<  description.productUri.data << endl;
        cout << "\n\tType: ";
        switch (description.applicationType) {
        case UA_APPLICATIONTYPE_SERVER:          cout << "Server";            break;
        case UA_APPLICATIONTYPE_CLIENT:          cout << "Client";            break;
        case UA_APPLICATIONTYPE_CLIENTANDSERVER: cout << "Client and Server"; break;
        case UA_APPLICATIONTYPE_DISCOVERYSERVER: cout << "Discovery Server";  break;
        default:                                 cout << "Unknown";
        }
        cout << endl <<  "\tDiscovery URLs:";
        for (size_t j = 0; j < description.discoveryUrlsSize; j++) {
            cout << endl << "\t\t" << j  << " " <<  description.discoveryUrls[j].length
                 << " " <<  description.discoveryUrls[j].data << endl;
        }
        cout << endl;
    }

    return 0;
}
