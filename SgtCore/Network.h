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

            const std::vector<Bus*>& busses() const
            {
                return busVec_;
            }

            const Bus* bus(const std::string& id) const
            {
                auto it = busMap_.find(id);
                return (it == busMap_.end()) ? nullptr : it->second.get();
            }
            Bus* bus(const std::string& id)
            {
                return const_cast<Bus*>((static_cast<const Network*>(this))->bus(id));
            }
            void addBus(std::shared_ptr<Bus> bus);

            const Bus* referenceBus() const
            {
                return referenceBus_;
            }
            Bus* referenceBus()
            {
                return referenceBus_;
            }
            void setReferenceBus(const std::string& id)
            {
                referenceBus_ = bus(id);
            }

            const std::vector<BranchAbc*>& branches() const
            {
                return branchVec_;
            }
            const BranchAbc* branch(const std::string& id) const
            {
                auto it = branchMap_.find(id);
                return (it == branchMap_.end()) ? nullptr : it->second.get();
            }
            BranchAbc* branch(const std::string& id)
            {
                return const_cast<BranchAbc*>((static_cast<const Network*>(this))->branch(id));
            }
            virtual void addBranch(std::shared_ptr<BranchAbc> branch,
                    const std::string& bus0Id, const std::string& bus1Id);

            const std::vector<GenAbc*>& gens() const
            {
                return genVec_;
            }
            const GenAbc* gen(const std::string& id) const
            {
                auto it = genMap_.find(id);
                return (it == genMap_.end()) ? nullptr : it->second.get();
            }
            GenAbc* gen(const std::string& id)
            {
                return const_cast<GenAbc*>((static_cast<const Network*>(this))->gen(id));
            }
            virtual void addGen(std::shared_ptr<GenAbc> gen, const std::string& busId);

            const std::vector<ZipAbc*>& zips() const
            {
                return zipVec_;
            }
            const ZipAbc* zip(const std::string& id) const
            {
                auto it = zipMap_.find(id);
                return (it == zipMap_.end()) ? nullptr : it->second.get();
            }
            ZipAbc* zip(const std::string& id)
            {
                return const_cast<ZipAbc*>((static_cast<const Network*>(this))->zip(id));
            }
            virtual void addZip(std::shared_ptr<ZipAbc> zip, const std::string& busId);

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

            std::map<std::string, std::shared_ptr<Bus>> busMap_;
            std::vector<Bus*> busVec_;
            Bus* referenceBus_{nullptr};

            std::map<std::string, std::shared_ptr<BranchAbc>> branchMap_;
            std::vector<BranchAbc*> branchVec_;

            std::map<std::string, std::shared_ptr<GenAbc>> genMap_;
            std::vector<GenAbc*> genVec_;

            std::map<std::string, std::shared_ptr<ZipAbc>> zipMap_;
            std::vector<ZipAbc*> zipVec_;

            std::unique_ptr<PowerFlowSolverInterface> solver_{nullptr};
            bool useFlatStart_{false};

            bool isValidSolution_{false};
    };
    
    inline std::ostream& operator<<(std::ostream& os, const Network& netw)
    {
        netw.print(os);
        return os;
    }

    std::unique_ptr<PowerFlowModel> buildModel(const Network& netw);
    void applyModel(const PowerFlowModel& mod, Network& netw);
}

#endif // NETWORK_DOT_H
