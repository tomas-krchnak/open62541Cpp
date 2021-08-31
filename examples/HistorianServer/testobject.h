#ifndef TESTOBJECT_H
#define TESTOBJECT_H
#include <open62541cpp/serverobjecttype.h>
#include "testmethod.h"

namespace opc = Open62541;

// Object Type Example
class TestObject : public opc::ServerObjectType
{
public:
    TestObject(opc::Server &s) : ServerObjectType(s,"TestObject") {}

    bool addChildren(const opc::NodeId& parent) override
    {
        opc::NodeId n;
        opc::NodeId a;
        addObjectTypeVariable<double>("Current", parent, n.notNull());
        addObjectTypeVariable<double>("Average", n, a.notNull());
        return true;
    }
};

#endif  // TESTOBJECT_H
