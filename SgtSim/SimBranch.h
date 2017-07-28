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
    /// @brief Simulation branch, corresponding to a BranchAbc in a SimNetwork's network(). 
    /// 
    /// branch() is provided by containment which is enough for a normal network branch types. 
    class SimBranch : virtual public SimComponent
    {
        public:

            SimBranch(const std::string& id, const ComponentPtr<BranchAbc>& branch) :
                Component(id),
                branch_(branch)
            {
                // Empty.
            }

            virtual const BranchAbc& branch() const
            {
                return *branch_;
            }
            
            virtual BranchAbc& branch()
            {
                return *branch_;
            }

        private:

            ComponentPtr<BranchAbc> branch_;
    };
    
    /// @brief Do anything needed to do to add simBranch to the simNetwork.
    ///
    /// Important: simBranch's branch must separately be added to simNetwork's network. This is to prevent any possible
    /// confusion about whether it is already added on not.
    void link(const ConstSimComponentPtr<SimBranch>& simBranch, SimNetwork& simNetwork);
}

#endif // SIM_BRANCH_DOT_H
