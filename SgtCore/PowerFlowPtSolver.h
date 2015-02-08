#ifndef POWER_FLOW_PT_SOLVER_DOT_H
#define POWER_FLOW_PT_SOLVER_DOT_H

#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/SgtPowerTools.h>

namespace SmartGridToolbox
{
   class Network;

   class PowerFlowPtSolver : public PowerFlowSolverInterface
   {
      public:
         virtual bool solve(Network* netw) override
         {
            runOpf(*netw);
            return true;
         }
   };
}

#endif // POWER_FLOW_PT_SOLVER_DOT_H
