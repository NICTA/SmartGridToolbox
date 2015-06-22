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
    class SimBus : virtual public SimComponent
    {
        public:

            SimBus(std::shared_ptr<Bus> bus) :
                Component(bus->id()),
                SimComponent(),
                bus_(bus)
            {
                // Empty.
            }

            std::shared_ptr<const Bus> bus() const
            {
                return bus_;
            }
            
            std::shared_ptr<Bus> bus()
            {
                return bus_;
            }

            virtual void joinNetwork(SimNetwork& simNetwork);

        protected:

            virtual void initializeState() override;

        private:

            std::shared_ptr<Bus> bus_;
    };
}

#endif // SIM_BUS_DOT_H
