#include "testobject.h"

namespace opc = Open62541;

bool TestObject::addChildren(const opc::NodeId &parent) {
    opc::NodeId ndAverage = addObjectTypeVariable("Average", parent, 48.8);
    opc::NodeId ndCurrent = addObjectTypeVariable("Current", parent, 50);

    return true;
}

bool DeviceObject::addChildren(const opc::NodeId &parent) {
    opc::NodeId ndFolder;
    opc::NodeId ndId;
    opc::NodeId ndType;
    addObjectTypeFolder("ID", parent, ndFolder.notNull());
    addObjectTypeVariable<int>("ID"  , ndFolder);
    addObjectTypeVariable<int>("Type", ndFolder);
    return true;
}

bool PumpObject::addChildren(const opc::NodeId &parent) {
    addObjectTypeVariable<int>("Current", parent);
    addObjectTypeVariable<int>("Average", parent);
    return true;
}
