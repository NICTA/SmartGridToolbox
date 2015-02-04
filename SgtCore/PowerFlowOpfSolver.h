#ifndef POWER_FLOW_OPF_SOLVER_DOT_H
#define POWER_FLOW_OPF_SOLVER_DOT_H

#include <SgtCore/PowerFlowSolver.h>

namespace SmartGridToolbox
{
   class PowerFlowModel;

   class PowerFlowOpfSolver : public PowerFlowSolverInterface
   {
      public:
         virtual void init(PowerFlowModel* mod) override;
         virtual bool solve() override;
   };
};

#endif // POWER_FLOW_OPF_SOLVER_ABC_DOT_H
