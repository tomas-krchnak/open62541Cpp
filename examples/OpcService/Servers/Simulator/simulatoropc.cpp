#include "simulatoropc.h"
#include <OpcServiceCommon/stockdefs.h>

namespace opc = Open62541;

SimulatorOpc* SimulatorOpc::_instance = nullptr;

/**
 * SimulatorOpc::initialise
 */
void SimulatorOpc::initialise()
{
    opc::Server::initialise();
    _idx = addNamespace("urn:simulator"); // create a name space
    opc::NodeId newFolder(_idx, "Simulator");

    if (addFolder(opc::NodeId::Objects, "Simulator", newFolder, opc::NodeId::Null)) {
        // periodic processing
        _process = std::make_unique<SimulateProcess>(*this,_idx);
        _process->start();

        // Make this server discoverable
        //if (!addPeriodicServerRegister(DISCOVERY_SERVER_ENDPOINT, _discoveryid)) {
        //    std::cerr << "Failed to register with discovery server" << endl;
        //}
    }

}
