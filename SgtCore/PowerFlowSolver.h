#ifndef POWER_FLOW_SOLVER_INTERFACE_DOT_H
#define POWER_FLOW_SOLVER_INTERFACE_DOT_H

namespace SmartGridToolbox
{
   class PowerFlowModel;

   class PowerFlowSolverInterface
   {
      public:
         virtual void init(PowerFlowModel* mod) = 0;
         virtual bool solve() = 0;
   };
};

#endif // POWER_FLOW_SOLVER_INTERFACE_DOT_H
