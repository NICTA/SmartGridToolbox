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

#ifndef SIM_GEN_DOT_H
#define SIM_GEN_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimNetwork.h>

#include <SgtCore/Gen.h>

namespace Sgt
{
    /// @brief Abstract base class for SimGen.
    /// 
    /// Depending on how the derived class works, gen() could either be provided by containment or inheritance.
    class SimGenAbc : virtual public SimComponent
    {
        public:

            /// @brief Return the gen that I wrap (const). 
            virtual const GenAbc& gen() const = 0;
            /// @brief Return the gen that I wrap (non-const). 
            virtual GenAbc& gen() = 0;
    };

    /// @brief Simulation gen, corresponding to a GenAbc in a SimNetwork's network(). 
    /// 
    /// gen() is provided by containment which is enough for a normal network gen types. 
    class SimGen : public SimGenAbc
    {
        public:

            SimGen(const std::string& id, const ComponentPtr<GenAbc>& gen) :
                Component(id),
                gen_(gen)
            {
                // Empty.
            }

            const GenAbc& gen() const override
            {
                return *gen_;
            }
            
            GenAbc& gen() override
            {
                return *gen_;
            }

        private:

            ComponentPtr<GenAbc> gen_;
    };

    /// @brief Do anything needed to do to add simGen to simNetwork.
    ///
    /// Important: simGen's gen must separately be added to simNetwork's network. This is to prevent any possible
    /// confusion about whether it is already added on not.
    void link(const ConstSimComponentPtr<SimGenAbc>& simGen, SimNetwork& simNetwork);
}

#endif // SIM_GEN_DOT_H
