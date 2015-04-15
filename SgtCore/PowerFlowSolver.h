#ifndef POWER_FLOW_SOLVER_INTERFACE_DOT_H
#define POWER_FLOW_SOLVER_INTERFACE_DOT_H

namespace Sgt
{
    class Network;

    class PowerFlowSolverInterface
    {
        public:
            virtual bool solve(Network* netw) = 0;
    };
};

#endif // POWER_FLOW_SOLVER_INTERFACE_DOT_H
