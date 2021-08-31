#ifndef SIMULATORNODECONTEXT_H
#define SIMULATORNODECONTEXT_H

#include <open62541cpp/open62541server.h>
#include <open62541cpp/nodecontext.h>

namespace opc = Open62541;

/**
 * The SimulatorNodeContext class
 */
class SimulatorNodeContext : public opc::NodeContext
{
public:
    SimulatorNodeContext() : opc::NodeContext("SimulatorWrite") {}

    virtual ~SimulatorNodeContext() {}
    /*!
        \brief readData
        \param node
        \param range
        \param value
        \return
    */
    virtual bool readData(opc::Server& server,
                          opc::NodeId& node,
                          const UA_NumericRange* range,
                          UA_DataValue& value);

    /*!
        \brief writeData
        \param server
        \param node
        \param range
        \param value
        \return
    */
    virtual bool writeData(opc::Server& server,
                           opc::NodeId& node,
                           const UA_NumericRange* range,
                           const UA_DataValue& value);
};

#endif  // SIMULATORNODECONTEXT_H
