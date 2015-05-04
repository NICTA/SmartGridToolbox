#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Component.h>
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
    typedef std::shared_ptr<const Bus> ConstBusPtr;
    typedef std::shared_ptr<Bus> BusPtr;

    typedef std::shared_ptr<const BranchAbc> ConstBranchPtr;
    typedef std::shared_ptr<BranchAbc> BranchPtr;

    typedef std::shared_ptr<const GenAbc> ConstGenPtr;
    typedef std::shared_ptr<GenAbc> GenPtr;

    typedef std::shared_ptr<const ZipAbc> ConstZipPtr;
    typedef std::shared_ptr<ZipAbc> ZipPtr;

    typedef std::vector<BusPtr> BusVec;
    typedef std::map<std::string, BusPtr> BusMap;

    typedef std::vector<BranchPtr> BranchVec;
    typedef std::map<std::string, BranchPtr> BranchMap;

    typedef std::vector<GenPtr> GenVec;
    typedef std::map<std::string, GenPtr> GenMap;

    typedef std::vector<ZipPtr> ZipVec;
    typedef std::map<std::string, ZipPtr> ZipMap;

    class PowerFlowModel;

    class Network : public Component
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("network");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            Network(const std::string& id, double PBase = 1.0);

        /// @}

        /// @name ComponentInterface virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual void print(std::ostream& os) const override;

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

            const BusVec& busses() const
            {
                return busVec_;
            }

            ConstBusPtr bus(const std::string& id) const
            {
                BusMap::const_iterator it = busMap_.find(id);
                return (it == busMap_.end()) ? nullptr : it->second;
            }
            BusPtr bus(const std::string& id)
            {
                return std::const_pointer_cast<Bus>((static_cast<const Network*>(this))->bus(id));
            }
            void addBus(BusPtr bus);

            ConstBusPtr referenceBus() const
            {
                return referenceBus_;
            }
            BusPtr referenceBus()
            {
                return referenceBus_;
            }
            void setReferenceBus(const std::string& id)
            {
                referenceBus_ = bus(id);
            }

            const BranchVec& branches() const
            {
                return branchVec_;
            }
            ConstBranchPtr branch(const std::string& id) const
            {
                BranchMap::const_iterator it = branchMap_.find(id);
                return (it == branchMap_.end()) ? nullptr : it->second;
            }
            BranchPtr branch(const std::string& id)
            {
                return std::const_pointer_cast<BranchAbc>((static_cast<const Network*>(this))->branch(id));
            }
            virtual void addBranch(BranchPtr branch, const std::string& bus0Id, const std::string& bus1Id);

            const GenVec& gens() const
            {
                return genVec_;
            }
            ConstGenPtr gen(const std::string& id) const
            {
                GenMap::const_iterator it = genMap_.find(id);
                return (it == genMap_.end()) ? nullptr : it->second;
            }
            GenPtr gen(const std::string& id)
            {
                return std::const_pointer_cast<GenAbc>((static_cast<const Network*>(this))->gen(id));
            }
            virtual void addGen(GenPtr gen, const std::string& busId);

            const ZipVec& zips() const
            {
                return zipVec_;
            }
            ConstZipPtr zip(const std::string& id) const
            {
                ZipMap::const_iterator it = zipMap_.find(id);
                return (it == zipMap_.end()) ? nullptr : it->second;
            }
            ZipPtr zip(const std::string& id)
            {
                return std::const_pointer_cast<ZipAbc>((static_cast<const Network*>(this))->zip(id));
            }
            virtual void addZip(ZipPtr zip, const std::string& busId);

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

            const PowerFlowSolverInterface* solver() const
            {
                return solver_.get();
            }
            
            PowerFlowSolverInterface* solver()
            {
                return solver_.get();
            }
            
            void setSolver(std::unique_ptr<PowerFlowSolverInterface> solver)
            {
                solver_ = std::move(solver);
            }

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

        private:

            double PBase_{1.0};
            double nomFreq_{50.0};
            double freq_{50.0};

            BusVec busVec_;
            BusMap busMap_;
            BusPtr referenceBus_{nullptr};

            BranchVec branchVec_;
            BranchMap branchMap_;

            GenVec genVec_;
            GenMap genMap_;

            ZipVec zipVec_;
            ZipMap zipMap_;

            std::unique_ptr<PowerFlowSolverInterface> solver_{nullptr};

            bool isValidSolution_{false};
    };

    std::unique_ptr<PowerFlowModel> buildModel(const Network& netw);
    void applyModel(const PowerFlowModel& mod, Network& netw);
}

#endif // NETWORK_DOT_H
