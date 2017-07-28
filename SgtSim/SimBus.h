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

#ifndef SIM_BUS_DOT_H
#define SIM_BUS_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimNetwork.h>

#include <SgtCore/Bus.h>

namespace Sgt
{
    /// @brief Simulation bus, corresponding to a Bus in a SimNetwork's network(). 
    class SimBus : virtual public SimComponent
    {
        public:

            SimBus(const std::string& id, const ComponentPtr<Bus>& bus) :
                Component(id),
                bus_(bus)
            {
                // Empty.
            }

            virtual const Bus& bus() const
            {
                return *bus_;
            }
            
            virtual Bus& bus()
            {
                return *bus_;
            }
        
        protected:

            virtual void initializeState() override;

        private:

            ComponentPtr<Bus> bus_;
    };

    /// @brief Do anything needed to do to add simBus to simNetwork.
    ///
    /// Important: simBus's bus must separately be added to simNetwork's network. This is to prevent any possible
    /// confusion about whether it is already added on not.
    void link(const ConstSimComponentPtr<SimBus>& simBus, SimNetwork& simNetwork);
}

#endif // SIM_BUS_DOT_H
