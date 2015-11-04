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

#ifndef SIM_BRANCH_DOT_H
#define SIM_BRANCH_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimNetwork.h>

#include <SgtCore/Branch.h>

#include <memory>

namespace Sgt
{
    /// @brief Abstract base class for SimBranch.
    /// 
    /// Depending on how the derived class works, branch() could either be provided by containment or inheritance.
    class SimBranchAbc : virtual public SimComponent
    {
        public:

            /// @brief Return the branch that I wrap (const). 
            virtual const BranchAbc& branch() const = 0;
            /// @brief Return the branch that I wrap (non-const). 
            virtual BranchAbc& branch() = 0;

            /// @brief Do anything I need to do to add myself to the simNetwork.
            ///
            /// Important: my branch must separately be added to SimNetwork's Network. This is to prevent any possible
            /// confusion about whether it is already added on not.
            virtual void linkToSimNetwork(SimNetwork& simNetwork);
    };

    /// @brief Simulation branch, corresponding to a BranchAbc in a SimNetwork's network(). 
    /// 
    /// branch() is provided by containment which is enough for a normal network branch types. 
    class SimBranch : public SimBranchAbc
    {
        public:

            SimBranch(BranchAbc& branch) :
                Component(branch.id()),
                branch_(&branch)
            {
                // Empty.
            }

            virtual const BranchAbc& branch() const override
            {
                return *branch_;
            }
            
            virtual BranchAbc& branch() override
            {
                return *branch_;
            }

        private:

            BranchAbc* branch_;
    };
}

#endif // SIM_BRANCH_DOT_H
