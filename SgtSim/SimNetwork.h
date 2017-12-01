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
    /// @brief Utility base class for branches in SimNetwork.
    ///
    /// Provides convenience accessors.
    /// Allows dependency ensuring I will update before network.
    class SimBranch : virtual public SimComponent
    {
        public:
        SimBranch(const ComponentPtr<BranchAbc>& branch) : branch_(branch) {};
        ConstComponentPtr<BranchAbc> branch() const {return branch_;}
        ComponentPtr<BranchAbc> branch() {return branch_;}
        private:
        ComponentPtr<BranchAbc> branch_;
    };

    /// @brief Utility base class for buses in SimNetwork.
    ///
    /// Provides convenience accessors.
    /// Allows dependency ensuring I will update before network.
    class SimBus : virtual public SimComponent
    {
        public:
        SimBus(const ComponentPtr<Bus>& bus) : bus_(bus) {};
        ConstComponentPtr<Bus> bus() const {return bus_;}
        ComponentPtr<Bus> bus() {return bus_;}
        private:
        ComponentPtr<Bus> bus_;
    };

    /// @brief Utility base class for gens in SimNetwork.
    ///
    /// Provides convenience accessors.
    /// Allows dependency ensuring I will update before network.
    class SimGen : virtual public SimComponent
    {
        public:
        SimGen(const ComponentPtr<Gen>& gen) : gen_(gen) {};
        ConstComponentPtr<Gen> gen() const {return gen_;}
        ComponentPtr<Gen> gen() {return gen_;}
        private:
        ComponentPtr<Gen> gen_;
    };

    /// @brief Utility base class for zips in SimNetwork.
    ///
    /// Provides convenience accessors.
    /// Allows dependency ensuring I will update before network.
    class SimZip : virtual public SimComponent
    {
        public:
        SimZip(const ComponentPtr<Zip>& zip) : zip_(zip) {};
        ConstComponentPtr<Zip> zip() const {return zip_;}
        ComponentPtr<Zip> zip() {return zip_;}

        private:
        ComponentPtr<Zip> zip_;
    };

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

        /// @brief Add a SimBranch to the SimNetwork.
        ///
        /// Creates dependency.
        void addSimBranch(ConstSimComponentPtr<SimBranch> simBranch)
        {
            dependsOn(simBranch, false);
        }

        /// @brief Add a SimBus to the SimNetwork.
        ///
        /// Creates dependency.
        void addSimBus(ConstSimComponentPtr<SimBus> simBus)
        {
            dependsOn(simBus, false);
        }

        /// @brief Add a SimGen to the SimNetwork.
        ///
        /// Creates dependency.
        void addSimGen(ConstSimComponentPtr<SimGen> simGen)
        {
            dependsOn(simGen, false);
        }

        /// @brief Add a SimZip to the SimNetwork.
        ///
        /// Creates dependency.
        void addSimZip(ConstSimComponentPtr<SimZip> simZip)
        {
            dependsOn(simZip, false);
        }

        protected:

        /// @name Overridden member functions from SimComponent.
        /// @{

        virtual void initializeState() override;

        virtual void updateState(const Time& t) override;

        /// @}

        private:

        std::shared_ptr<Network> network_;
        Event networkChanged_;
    };
}

#endif // SIM_NETWORK_DOT_H
