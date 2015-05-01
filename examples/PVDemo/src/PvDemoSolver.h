#ifndef PV_DEMO_DOT_H
#define PV_DEMO_DOT_H

#include <SgtCore/PowerFlowPtPpSolver.h>

#include <PowerTools++/var.h>

class PowerModel;

namespace Sgt
{
    class Network;
    class Simulation;

    class PvDemoSolver : public PowerFlowPtPpSolver
    {
        protected:
            virtual std::unique_ptr<PowerModel> makeModel() override;

        private:
            var<double> VBoundViol_;
    };
}

#endif // PV_DEMO_DOT_H
