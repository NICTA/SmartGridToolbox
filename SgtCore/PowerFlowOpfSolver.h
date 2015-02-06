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
         virtual void bindNetwork(Network* netw)
         {
            netw_ = netw;
         }

         virtual bool solve()
         {
            runOpf(*netw_);
            return true;
         }
      private:
         Network* netw_;
   };
}

#endif // POWER_FLOW_OPF_SOLVER_DOT_H
