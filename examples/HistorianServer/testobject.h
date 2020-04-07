#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <serverobjecttype.h>
#include "testmethod.h"

namespace opc = Open62541;

// Object Type Example
class TestObject : public opc::ServerObjectType
{
public:
    TestObject(opc::Server &s) : ServerObjectType(s,"TestObject") {

    }

    virtual bool addChildren(opc::NodeId &parent) {
        opc::NodeId n;
        opc::NodeId a;
        addObjectTypeVariable<double>("Current", parent, n.notNull());
        addObjectTypeVariable<double>("Average", n, a.notNull());
        return true;
    }
};

#endif // TESTOBJECT_H
