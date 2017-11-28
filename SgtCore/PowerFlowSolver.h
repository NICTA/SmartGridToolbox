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

#ifndef POWER_FLOW_SOLVER_INTERFACE_DOT_H
#define POWER_FLOW_SOLVER_INTERFACE_DOT_H

namespace Sgt
{
    class Network;

    /// @brief Interface for power flow solvers operating on Network objects.
    /// @ingroup PowerFlowCore
    class PowerFlowSolverInterface
    {
        public:
        virtual ~PowerFlowSolverInterface() = default;
        virtual bool solve(Network& netw) = 0;
    };
};

#endif // POWER_FLOW_SOLVER_INTERFACE_DOT_H
