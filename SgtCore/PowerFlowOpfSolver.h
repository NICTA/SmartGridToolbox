#ifndef POWER_FLOW_OPF_SOLVER_DOT_H
#define POWER_FLOW_OPF_SOLVER_DOT_H

#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/SgtPowerTools.h>

namespace SmartGridToolbox
{
   class Network;

   class PowerFlowOpfSolver : public PowerFlowSolverInterface
   {
      public:
         virtual bool solve(Network* netw) override
         {
            runOpf(*netw);
            return true;
         }
   };
}

#endif // POWER_FLOW_OPF_SOLVER_DOT_H
