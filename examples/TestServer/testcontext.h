#ifndef TESTCONTEXT_H
#define TESTCONTEXT_H
#include <nodecontext.h>
#include <iostream>
using namespace std;
//
// Base class for example case of using a NodeContext to hook on to node callbacks
// This prints trace messages when the functions are called
// Note how the class is registered in the source file
//
class TestContext : public Open62541::NodeContext
{
public:
    TestContext() : Open62541::NodeContext("TestContext")
    {
        cout << "Register Node Context " << name() << endl;
    }

    //
    // Global constructor / destructor
    virtual bool construct(Open62541::Server &server,  Open62541::NodeId &node) {
        cout << "Global Constructor " << name() << endl;
        return NodeContext::construct(server,node); // doing nothing is OK
    }

    /**
     * destruct
     */
    virtual void destruct(Open62541::Server &server,  Open62541::NodeId &node) {
        cout << "Global Destructor " << name() << endl;
        NodeContext::destruct(server,node);
    }

    //
    // Object Type Lifecycle Construct and Destruct
    //

    /**
     * typeConstruct
     * @return 
     */
   virtual bool typeConstruct(Open62541::Server &server, Open62541::NodeId &node, Open62541::NodeId &typeNode)
   {
        cout << " Object Type Constructor " << name() << endl;
       return NodeContext::typeConstruct(server,node,typeNode);
   }

    /**
     * typeDestruct
     * @param server
     * @param n
     */
    virtual void  typeDestruct(Open62541::Server &server, Open62541::NodeId &node, Open62541::NodeId &typeNode)
    {
        cout << " Object Type Destructor " << name() << endl;
        NodeContext::typeDestruct(server,node,typeNode);
    }


    //
    // Data Read and Write
    //
    /**
     * readData
     * @param node
     * @param range
     * @param value
     * @return 
     */
    virtual bool readData(Open62541::Server &/*server*/, Open62541::NodeId &node, const UA_NumericRange* /*range*/, UA_DataValue& /*value*/) {
        cout << __FUNCTION__ << " " << name() << " NodeId " << Open62541::toString(node.get()) <<  endl;
        return false;
    }

    /**
     * writeData
     * @param server
     * @param node
     * @param range
     * @param value
     * @return 
     */
    virtual bool writeData(Open62541::Server &/*server*/,  Open62541::NodeId &node, const UA_NumericRange */*range*/, const UA_DataValue &/*value*/) {
        cout << __FUNCTION__ << " " << name() << " NodeId " << Open62541::toString(node.get()) <<  endl;
        return false;
    }

    // Value Read / Write callbacks
    /**
     * readValue
     * @param node
     */
    virtual void readValue(Open62541::Server &/*server*/, Open62541::NodeId &node, const UA_NumericRange */*range*/, const UA_DataValue */*value*/)
    {
        cout << __FUNCTION__ << " " << name() << " NodeId " << Open62541::toString(node.get()) <<  endl;
    }

    /**
     * writeValue
     * @param node
     */
    virtual void writeValue(Open62541::Server &/*server*/, Open62541::NodeId &node, const UA_NumericRange */*range*/, const UA_DataValue &/*value*/)
    {
        cout << __FUNCTION__ << " " << name() << " NodeId " << Open62541::toString(node.get()) <<  endl;
    }


};

#endif // TESTCONTEXT_H
