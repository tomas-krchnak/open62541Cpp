#include "testobject.h"

namespace opc = Open62541;

bool TestObject::addChildren(const opc::NodeId &parent) {
    opc::NodeId ndCurrent;
    opc::NodeId ndAverage;
    addObjectTypeVariable<double>("Current", parent,    ndCurrent.notNull());
    addObjectTypeVariable<double>("Average", ndCurrent, ndAverage.notNull());
    return true;
}

bool DeviceObject::addChildren(const opc::NodeId &parent) {
    opc::NodeId ndCurrent;
    opc::NodeId ndAverage;
    addObjectTypeVariable<double>("Current", parent, ndCurrent.notNull());
    addObjectTypeVariable<double>("Average", ndCurrent, ndAverage.notNull());
    return true;
}

bool PumpObject::addChildren(const opc::NodeId &parent) {
    opc::NodeId ndCurrent;
    opc::NodeId ndAverage;
    addObjectTypeVariable<double>("Current", parent,    ndCurrent.notNull());
    addObjectTypeVariable<double>("Average", ndCurrent, ndAverage.notNull());
    return true;
}
