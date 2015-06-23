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

#include <memory>

namespace Sgt
{
    /// @brief Abstract base class for SimBus.
    /// 
    /// Depending on how the derived class works, bus() could either be provided by containment or inheritance.
    class SimBusAbc : virtual public SimComponent
    {
        public:

            virtual std::shared_ptr<const Bus> bus() const = 0;
            virtual std::shared_ptr<Bus> bus() = 0;
            
            virtual void joinNetwork(SimNetwork& simNetwork);

        protected:

            virtual void initializeState() override;
    };

    /// @brief Simulation bus, corresponding to a Bus in a SimNetwork's network(). 
    /// 
    /// bus() is provided by containment which is enough for a normal network bus. 
    class SimBus : public SimBusAbc
    {
        public:

            SimBus(std::shared_ptr<Bus> bus) :
                Component(bus->id()),
                SimComponent(),
                bus_(bus)
            {
                // Empty.
            }

            std::shared_ptr<const Bus> bus() const override
            {
                return bus_;
            }
            
            std::shared_ptr<Bus> bus() override
            {
                return bus_;
            }


        private:

            std::shared_ptr<Bus> bus_;
    };
}

#endif // SIM_BUS_DOT_H
