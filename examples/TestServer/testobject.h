#ifndef TESTOBJECT_H
#define TESTOBJECT_H
#include <open62541cpp/serverobjecttype.h>
#include "testmethod.h"

namespace opc = Open62541;

class TestObject : public opc::ServerObjectType
{
public:
    TestObject(opc::Server& s) : ServerObjectType(s, "TestObject") {}


    bool addChildren(const Open62541::NodeId& parent) override
    {
        Open62541::NodeId n;
        Open62541::NodeId a;
        return addObjectTypeVariable<double>("Current", parent, n.notNull()) &&
               addObjectTypeVariable<double>("Average", parent, a.notNull());
    }
};

class DeviceObject : public opc::ServerObjectType {
public:
    DeviceObject(opc::Server& s) : ServerObjectType(s, "DeviceObject") {}
    bool addChildren(const opc::NodeId &parent) override; // ServerObjectType
};

class PumpObject : public opc::ServerObjectType {
public:
    PumpObject(opc::Server& s) : ServerObjectType(s, "PumpObject") {}
    bool addChildren(const opc::NodeId& parent) override; // ServerObjectType
};

#endif  // TESTOBJECT_H
