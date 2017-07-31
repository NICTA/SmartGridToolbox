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

            SimNetwork(const std::string& id, std::shared_ptr<Network> network);

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; // TODO

            /// @}

            /// @name Network access.
            /// @{

            const Network& network() const
            {
                return *network_;
            }

            Network& network()
            {
                return *network_;
            }

            /// @}
            
            /// @name Network modification.
            /// @{
           
            /// @brief Link a new branch to the SimNetwork.
            ///
            /// Note that all branches are linked automatically at construction time, so this only need be called
            /// if a new branch is subsequently added to the network.
            void linkBranch(const BranchAbc& branch);

            /// @brief Link a new bus to the SimNetwork.
            ///
            /// Note that all buses are linked automatically at construction time, so this only need be called
            /// if a new bus is subsequently added to the network.
            void linkBus(const Bus& bus);
            
            /// @brief Link a new gen to the SimNetwork.
            ///
            /// Note that all gens are linked automatically at construction time, so this only need be called
            /// if a new gen is subsequently added to the network.
            void linkGen(const Gen& gen);
           
            /// @brief Link a new zip to the SimNetwork.
            ///
            /// Note that all zips are linked automatically at construction time, so this only need be called
            /// if a new zip is subsequently added to the network.
            void linkZip(const Zip& zip);

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

    /// @brief Utility base class for branches in SimNetwork.
    class SimBranch
    {
        public:
            SimBranch(const ComponentPtr<BranchAbc>& branch, SimNetwork& simNetwork);
            ConstComponentPtr<BranchAbc> branch() const {return branch_;}
            ComponentPtr<BranchAbc> branch() {return branch_;}
        private:
            ComponentPtr<BranchAbc> branch_;
    };

    /// @brief Utility base class for buses in SimNetwork.
    class SimBus
    {
        public:
            SimBus(const ComponentPtr<Bus>& bus, SimNetwork& simNetwork);
            ConstComponentPtr<Bus> bus() const {return bus_;}
            ComponentPtr<Bus> bus() {return bus_;}
        private:
            ComponentPtr<Bus> bus_;
    };

    /// @brief Utility base class for gens in SimNetwork.
    class SimGen
    {
        public:
            SimGen(const ComponentPtr<Gen>& gen, SimNetwork& simNetwork);
            ConstComponentPtr<Gen> gen() const {return gen_;}
            ComponentPtr<Gen> gen() {return gen_;}
        private:
            ComponentPtr<Gen> gen_;
    };

    /// @brief Utility base class for zips in SimNetwork.
    class SimZip
    {
        public:
            SimZip(const ComponentPtr<Zip>& zip, SimNetwork& simNetwork);
            ConstComponentPtr<Zip> zip() const {return zip_;}
            ComponentPtr<Zip> zip() {return zip_;}
        private:
            ComponentPtr<Zip> zip_;
    };
}

#endif // SIM_NETWORK_DOT_H
