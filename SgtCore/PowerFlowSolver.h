#ifndef POWER_FLOW_SOLVER_INTERFACE_DOT_H
#define POWER_FLOW_SOLVER_INTERFACE_DOT_H

namespace Sgt
{
    class Network;

    class PowerFlowSolverInterface
    {
        public:
            virtual ~PowerFlowSolverInterface() {}

            bool solve(Network* netw);

            virtual void setNetwork(Network* netw) = 0;

            virtual bool solveProblem() = 0;

            virtual void updateNetwork() = 0;
    };
};

#endif // POWER_FLOW_SOLVER_INTERFACE_DOT_H
