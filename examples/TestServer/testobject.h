#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <serverobjecttype.h>
#include "testmethod.h"

namespace opc = Open62541;

class TestObject : public opc::ServerObjectType
{
public:
    TestObject(opc::Server& s) : ServerObjectType(s, "TestObject") {}
    bool addChildren(opc::NodeId& parent) override; // ServerObjectType
};

class DeviceObject : public opc::ServerObjectType {
public:
    DeviceObject(opc::Server& s) : ServerObjectType(s, "DeviceObject") {}
    bool addChildren(opc::NodeId &parent) override; // ServerObjectType
};

class PumpObject : public opc::ServerObjectType {
public:
    PumpObject(opc::Server& s) : ServerObjectType(s, "PumpObject") {}
    bool addChildren(opc::NodeId& parent) override; // ServerObjectType
};

#endif // TESTOBJECT_H
