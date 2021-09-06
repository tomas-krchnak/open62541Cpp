#ifndef TESTOBJECT_H
#define TESTOBJECT_H
#include <open62541cpp/serverobjecttype.h>
#include <open62541cpp/propertytree.h>
#include <open62541cpp/open62541objects.h>
#include "testmethod.h"

namespace opc = Open62541;

class TestObject : public opc::ServerObjectType
{
public:
    TestObject(opc::Server& s) : ServerObjectType(s, "TestObject") {}


    bool addChildren(const opc::NodeId& parent) override
    {
        opc::NodeId n;
        opc::NodeId a;
        opc::NodeId b;
        return addObjectTypeVariable<double>("Current", parent, n.notNull()) &&
               addDerivedObjectType ("Golash", parent, a.notNull())&&
               addObjectTypeArrayVariable<int, 5>("Average", a, b.notNull());
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
