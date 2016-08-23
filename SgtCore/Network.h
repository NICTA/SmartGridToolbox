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

#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Components.h>
#include <SgtCore/Gen.h>
#include <SgtCore/PowerFlow.h>
#include <SgtCore/PowerFlowSolver.h>
#include <SgtCore/Zip.h>

#include<iostream>
#include<map>
#include<memory>
#include<vector>

namespace Sgt
{
    class PowerFlowModel;

    struct Island
    {
        int idx{-1};
        bool isSupplied{false};
        std::vector<ComponentPtr<Bus>> buses;
    };
    
    struct ConstIsland
    {
        ConstIsland(const Island& from) :
            idx(from.idx),
            isSupplied(from.isSupplied),
            buses({from.buses.begin(), from.buses.end()})
        {
            // Empty.
        }

        int idx{-1};
        bool isSupplied{false};
        std::vector<ConstComponentPtr<Bus>> buses;
    };

    /// @brief Network component, describing an electricity network.
    /// @ingroup PowerFlowCore
    class Network
    {
        public:

            /// @name Lifecycle:
            /// @{

            Network(double PBase = 1.0);

            virtual ~Network() = default;

            /// @}

            /// @name Network Attributes:
            /// @{

            double PBase() const
            {
                return PBase_;
            }

            virtual void setPBase(double PBase)
            {
                PBase_ = PBase;
            }

            double nomFreq() const
            {
                return nomFreq_;
            }

            virtual void setNomFreq(double nomFreq)
            {
                nomFreq_ = nomFreq;
            }

            double freq() const
            {
                return freq_;
            }

            virtual void setFreq(double freq)
            {
                freq_ = freq;
            }

            /// @}

            /// @name Network Components:
            /// @{

            const Components<Bus>& buses() const
            {
                return buses_;
            }
            
            Components<Bus>& buses()
            {
                return buses_;
            }

            ComponentPtr<Bus> addBus(std::shared_ptr<Bus> bus)
            {
                return buses_.insert(bus);
            }
            
            std::shared_ptr<Bus> removeBus(std::string& id)
            {
                return buses_.remove(id);
            }

            ConstComponentPtr<Bus> referenceBus() const
            {
                return referenceBus_;
            }
            ComponentPtr<Bus> referenceBus()
            {
                return referenceBus_;
            }

            void setReferenceBus(const std::string& id)
            {
                referenceBus_ = buses_[id];
            }

            const Components<BranchAbc>& branches() const
            {
                return branches_;
            }
            
            Components<BranchAbc>& branches()
            {
                return branches_;
            }

            ComponentPtr<BranchAbc> addBranch(std::shared_ptr<BranchAbc> branch,
                    const std::string& bus0Id, const std::string& bus1Id);
            
            std::shared_ptr<BranchAbc> removeBranch(std::string& id)
            {
                return branches_.remove(id);
            }

            const Components<GenAbc>& gens() const
            {
                return gens_;
            }
            
            Components<GenAbc>& gens()
            {
                return gens_;
            }

            ComponentPtr<GenAbc> addGen(std::shared_ptr<GenAbc> gen, const std::string& busId);
            
            std::shared_ptr<GenAbc> removeGen(std::string& id)
            {
                return gens_.remove(id);
            }

            const Components<ZipAbc>& zips() const
            {
                return zips_;
            }
            
            Components<ZipAbc>& zips()
            {
                return zips_;
            }

            ComponentPtr<ZipAbc> addZip(std::shared_ptr<ZipAbc> zip, const std::string& busId);
            
            std::shared_ptr<ZipAbc> removeZip(std::string& id)
            {
                return zips_.remove(id);
            }

            /// @}

            /// @name Per-Unit Conversions:
            /// @{

            template<typename T> T V2Pu(const T& V, double VBase) const
            {
                return V / VBase;
            }
            template<typename T> T pu2V(const T& pu, double VBase) const
            {
                return pu * VBase;
            }

            template<typename T> T S2Pu(const T& S) const
            {
                return S / PBase_;
            }
            template<typename T> T pu2S(const T& pu) const
            {
                return pu * PBase_;
            }

            template<typename T> T Y2Pu(const T& Y, double VBase) const
            {
                return Y * VBase * VBase / PBase_;
            }
            template<typename T> T pu2Y(const T& pu, double VBase) const
            {
                return pu * PBase_ / (VBase * VBase);
            }

            template<typename T> T Z2Pu(const T& Z, double VBase) const
            {
                return Z * PBase_ / (VBase * VBase);
            }
            template<typename T> T pu2Z(const T& pu, double VBase) const
            {
                return pu * VBase * VBase / PBase_;
            }

            template<typename T> T I2Pu(const T& I, double VBase) const
            {
                return I * VBase / PBase_;
            }
            template<typename T> T pu2I(const T& pu, double VBase) const
            {
                return pu * PBase_ / VBase;
            }

            /// @}

            /// @name Power-Flow problem:
            /// @{

            const PowerFlowSolverInterface& solver() const
            {
                return *solver_;
            }

            PowerFlowSolverInterface& solver()
            {
                return *solver_;
            }

            void setSolver(std::unique_ptr<PowerFlowSolverInterface> solver)
            {
                solver_ = std::move(solver);
            }
    
            bool useFlatStart() const
            {
                return useFlatStart_;
            }

            void setUseFlatStart(bool useFlatStart)
            {
                useFlatStart_ = useFlatStart;
            }
            
            void applyFlatStart();

            virtual bool solvePowerFlow();

            bool isValidSolution() const
            {
                return isValidSolution_;
            }

            /// @}

            /// @name Generation cost:
            /// @{

            double genCostPerUnitTime();

            /// @}
            
            /// @name Connectivity / islands:
            /// @{
          
            std::vector<ConstIsland> islands() const
            {
                return {islands_.begin(), islands_.end()};
            }

            const std::vector<Island>& islands()
            {
                return islands_;
            }

            void findIslands();
            
            void handleUnsuppliedIslands();

            /// @}
            
            /// @name Printing:
            /// @{
           
            virtual void print(std::ostream& os) const
            {
                os << toJson().dump(2);
            }

            virtual json toJson() const; 
            
            /// @}

        private:

            double PBase_{1.0};
            double nomFreq_{50.0};
            double freq_{50.0};

            MutableComponents<Bus> buses_;
            ComponentPtr<Bus> referenceBus_;
            
            MutableComponents<BranchAbc> branches_;
            
            MutableComponents<GenAbc> gens_;

            MutableComponents<ZipAbc> zips_;

            std::vector<Island> islands_;

            std::unique_ptr<PowerFlowSolverInterface> solver_{nullptr};
            bool useFlatStart_{false};

            bool isValidSolution_{false};
    };
    
    inline std::ostream& operator<<(std::ostream& os, const Network& netw)
    {
        netw.print(os);
        return os;
    }

    std::unique_ptr<PowerFlowModel> buildModel(const Network& netw,
            const std::function<bool (const Bus&)> selBus = [](const Bus&){return true;});

    void applyModel(const PowerFlowModel& mod, Network& netw);
}

#endif // NETWORK_DOT_H
