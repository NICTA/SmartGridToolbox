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

#ifndef SIM_NETWORK_DOT_H
#define SIM_NETWORK_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace Sgt
{
    /// @brief SimNetwork : A SimComponent for an electrical network.
    class SimNetwork : virtual public SimComponent
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("sim_network");
                return result;
            }

        /// @}

        /// @name Lifecycle.
        /// @{

            SimNetwork(const std::string& id, std::shared_ptr<Network> network) : 
                Component(id), network_(network) {}

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}

        /// @name Network access.
        /// @{

            const Network* network() const
            {
                return network_.get();
            }

            Network* network()
            {
                return network_.get();
            }

        /// @}

        /// @name Overridden member functions from SimComponent.
        /// @{

        public:

            // virtual Time validUntil() const override;

        protected:

            // virtual void initializeState() override;
            virtual void updateState(Time t) override;

        /// @}

        private:

            std::shared_ptr<Network> network_;
    };
}

#endif // SIM_NETWORK_DOT_H
