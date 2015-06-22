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
#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Network.h>

namespace Sgt
{
    /// @brief SimNetwork : A SimComponent for an electrical network.
    class SimNetwork : public SimComponent
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

            SimNetwork(const std::string& id, std::shared_ptr<Network> network) : SimComponent(id), network_(network) {}

        /// @}

        /// @name Component virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}

        /// @name Network access.
        /// @{

            std::shared_ptr<const Network> network() const
            {
                return network_;
            }

            std::shared_ptr<Network> network()
            {
                return network_;
            }

        /// @}

        /// @name Adding components.
        /// @{

        public:

            template<typename T> void addBus(std::shared_ptr<T> simBus)
            {
                auto networkBus = network_->bus(simBus->bus()->id());
                if (!networkBus)
                {
                    network_->addBus(simBus->bus());
                }
                else
                {
                    sgtAssert(networkBus == simBus, 
                            "Bus " << networkBus->id() << " already exists in the network.");
                }

                dependsOn(simBus);
                simBus->bus()->setpointChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simBus->bus()->isInServiceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            }

            template<typename T> void addBranch(std::shared_ptr<T> simBranch, const std::string& bus0Id,
                                                const std::string& bus1Id)
            {
                auto networkBranch = network_->branch(simBranch->branch()->id());
                if (!networkBranch)
                {
                    network_->addBranch(simBranch->branch());
                }
                else
                {
                    sgtAssert(networkBranch == simBranch, 
                            "Branch " << networkBranch->id() << " already exists in the network.");
                }

                dependsOn(simBranch);
                simBranch->branch()->admittanceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simBranch->branch()->isInServiceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            }

            template<typename T> void addGen(std::shared_ptr<T> simGen, const std::string& busId)
            {
                auto networkGen = network_->gen(simGen->gen()->id());
                if (!networkGen)
                {
                    network_->addGen(simGen->gen());
                }
                else
                {
                    sgtAssert(networkGen == simGen, 
                            "Gen " << networkGen->id() << " already exists in the network.");
                }
                
                dependsOn(simGen);
                simGen->gen()->setpointChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simGen->gen()->isInServiceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            }

            template<typename T> void addZip(std::shared_ptr<T> simZip, const std::string& busId)
            {
                auto networkZip = network_->zip(simZip->zip()->id());
                if (!networkZip)
                {
                    network_->addZip(simZip->zip());
                }
                else
                {
                    sgtAssert(networkZip == simZip, 
                            "Zip " << networkZip->id() << " already exists in the network.");
                }
               
                dependsOn(simZip);
                simZip->zip()->injectionChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simZip->zip()->setpointChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simZip->zip()->isInServiceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
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
