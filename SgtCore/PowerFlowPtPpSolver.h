#ifndef POWER_FLOW_PT_PP_SOLVER_DOT_H
#define POWER_FLOW_PT_PP_SOLVER_DOT_H

#include <SgtCore/Network.h>
#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/SgtPowerTools.h>

class Net;
class PowerModel;

#include <memory>

namespace Sgt
{
    class Network;

    class PowerFlowPtPpSolver : public PowerFlowSolverInterface
    {
        public:
            virtual bool solve(Network* netw) override;
        protected:
            std::unique_ptr<PowerModel> getModel(const Network& sgtNetw, Net& ptNetw);
    };
}

#endif // POWER_FLOW_PT_PP_SOLVER_DOT_H
