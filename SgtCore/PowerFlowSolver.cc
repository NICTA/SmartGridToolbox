#include "PowerFlowSolver.h"

namespace Sgt
{
    bool PowerFlowSolverInterface::solve(Network* netw)
    {
        setNetwork(netw);
        bool ok = solveProblem();
        if (ok)
        {
            updateNetwork();
        }
        return ok;
    }
}
