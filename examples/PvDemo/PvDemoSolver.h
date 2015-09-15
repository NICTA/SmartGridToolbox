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

#ifndef PV_DEMO_SOLVER_DOT_H
#define PV_DEMO_SOLVER_DOT_H

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
            var<> V2Slack_;
    };
}

#endif // PV_DEMO_SOLVER_DOT_H
