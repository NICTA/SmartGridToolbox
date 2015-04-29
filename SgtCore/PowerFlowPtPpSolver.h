#ifndef POWER_FLOW_PT_PP_SOLVER_DOT_H
#define POWER_FLOW_PT_PP_SOLVER_DOT_H

#include <SgtCore/Network.h>
#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/SgtPowerTools.h>

class PowerModel;
class Net;

#include <memory>

namespace Sgt
{
    class Network;

    class PowerFlowPtPpSolver : public PowerFlowSolverInterface
    {
        public:

            virtual ~PowerFlowPtPpSolver();

            virtual void setNetwork(Network* netw) override;

            virtual bool solveProblem() override;

            virtual void updateNetwork() override;

        
        protected:
            virtual std::unique_ptr<PowerModel> makeModel();
        
        protected:
            Network * sgtNetw_{nullptr};
            Net* ptNetw_{nullptr};
    };
}

#endif // POWER_FLOW_PT_PP_SOLVER_DOT_H
