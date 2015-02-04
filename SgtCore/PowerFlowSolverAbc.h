#ifndef POWER_FLOW_SOLVER_ABC_DOT_H
#define POWER_FLOW_SOLVER_ABC_DOT_H

namespace SmartGridToolbox
{
   class PowerFlowModel;

   class PowerFlowSolverAbc
   {
      public:
         PowerFlowSolverAbc(PowerFlowModel* mod) :
            mod_(mod)
         {
            // Empty.
         }

         virtual bool solve() = 0;
      
      private:
         PowerFlowModel* mod_;
   };
};

#endif // POWER_FLOW_SOLVER_ABC_DOT_H
