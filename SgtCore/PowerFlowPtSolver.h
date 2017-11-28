// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef POWER_FLOW_PT_SOLVER_DOT_H
#define POWER_FLOW_PT_SOLVER_DOT_H

#include <SgtCore/Network.h>
#include <SgtCore/PowerFlowSolver.h>

#include <PowerTools++/Net.h>

class PowerModel;
class Net;

#include <memory>

namespace Sgt
{
    class Network;

    /// @brief Optimal power flow (OPF) solver.
    ///
    /// Uses the external PowerTools library.
    /// @ingroup PowerFlowCore
    class PowerFlowPtSolver : public PowerFlowSolverInterface
    {
        public:

        PowerFlowPtSolver();
        virtual ~PowerFlowPtSolver();

        virtual bool solve(Network& netw) override;

        protected:
        virtual std::unique_ptr<PowerModel> makeModel();

        protected:
        Network* sgtNetw_{nullptr};
        std::unique_ptr<Net> ptNetw_{nullptr};
    };
}

#endif // POWER_FLOW_PT_SOLVER_DOT_H
