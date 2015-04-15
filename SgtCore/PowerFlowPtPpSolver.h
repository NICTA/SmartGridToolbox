#ifndef POWER_FLOW_PT_PP_SOLVER_DOT_H
#define POWER_FLOW_PT_PP_SOLVER_DOT_H

#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/SgtPowerTools.h>

namespace Sgt
{
    class Network;

    class PowerFlowPtPpSolver : public PowerFlowSolverInterface
    {
        public:
            virtual bool solve(Network* netw) override;
    };
}

#endif // POWER_FLOW_PT_PP_SOLVER_DOT_H
