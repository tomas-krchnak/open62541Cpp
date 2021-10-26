#ifndef SIMULATEPROCESS_H
#define SIMULATEPROCESS_H

#include <open62541cpp/open62541server.h>
#include <open62541cpp/serverrepeatedcallback.h>
#include <OpcServiceCommon/opcservicecommon.h>
#include "simulatordefs.h"
#include <OpcServiceCommon/stockdefs.h>
#include "simulatornodecontext.h"
#include "simulatorstartmethod.h"
#include "simulatorstopmethod.h"

namespace opc = Open62541;

enum
{
    ValueId = 1000,
    StatusId,
    RangeId,
    TypeId,
    IntervalId
};

/**
 * The SimulateProcess class
 * This is a data collection process driven on a timer
 */
class SimulateProcess : public opc::ServerRepeatedCallback {
    int     _ticks      = 0;
    int     _lastValue  = 0;    /**< the last generated value */
    bool    _dirUp      = true; /**< ramp direction */
    int     _idx;               /**< The namespace */
    //
    SimulatorNodeContext _context;
    SimulatorStartMethod _startMethod;
    SimulatorStopMethod _stopMethod;
    //
    // The node ids used
    //
public:
    // Node references
    opc::NodeId Value;
    opc::NodeId Status;
    opc::NodeId Range;
    opc::NodeId Type;
    opc::NodeId Interval;

   /**
    * SimulateProcess
    * @param s
    */
    SimulateProcess(opc::Server &s, int ns = 2);

   /**
    * callback
    */
    void callback();
};


#endif // SIMULATEPROCESS_H
