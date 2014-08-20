#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include<SgtCore/Branch.h>
#include<SgtCore/Bus.h>
#include<SgtCore/Gen.h>
#include<SgtCore/Zip.h>

#include<iostream>
#include<vector>

namespace SmartGridToolbox
{
   class Network : public Component
   {
      public:

         typedef std::vector<std::shared_ptr<Bus>> BusVec;
         typedef std::map<std::string, std::shared_ptr<Bus>> BusMap;

         typedef std::vector<std::shared_ptr<Branch>> BranchVec;
         typedef std::map<std::string, std::shared_ptr<Branch>> BranchMap;

         typedef std::vector<std::shared_ptr<Gen>> GenVec;
         typedef std::map<std::string, std::shared_ptr<Gen>> GenMap;

         typedef std::vector<std::shared_ptr<Zip>> ZipVec;
         typedef std::map<std::string, std::shared_ptr<Zip>> ZipMap;
         

      /// @name Lifecycle:
      /// @{
         
         Network(const std::string& id, double PBase);

         virtual ~Network() = default;

      /// @}
      
      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "network";}

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
         std::shared_ptr<const Bus> bus(const std::string& id) const
         {
            BusMap::const_iterator it = busMap_.find(id);
            return (it == busMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Bus> bus(const std::string& id)
         {
            return std::const_pointer_cast<Bus>((const_cast<const Network*>(this))->bus(id));
         }
         virtual void addBus(std::shared_ptr<Bus> bus)
         {
            busMap_[bus->id()] = bus;
            busVec_.push_back(bus);
         }

         const BranchVec& branches() const
         {
            return branchVec_;
         }
         std::shared_ptr<const Branch> branch(const std::string& id) const
         {
            BranchMap::const_iterator it = branchMap_.find(id);
            return (it == branchMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Branch> branch(const std::string& id)
         {
            return std::const_pointer_cast<Branch>((const_cast<const Network*>(this))->branch(id));
         }
         virtual void addBranch(std::shared_ptr<Branch> branch)
         {
            branchMap_[branch->id()] = branch;
            branchVec_.push_back(branch);
         }

         const GenVec& gens() const
         {
            return genVec_;
         }
         std::shared_ptr<const Gen> gen(const std::string& id) const
         {
            GenMap::const_iterator it = genMap_.find(id);
            return (it == genMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Gen> gen(const std::string& id)
         {
            return std::const_pointer_cast<Gen>((const_cast<const Network*>(this))->gen(id));
         }
         virtual void addGen(std::shared_ptr<Gen> gen, Bus& toBus)
         {
            genMap_[gen->id()] = gen;
            toBus.addGen(gen);
            genVec_.push_back(gen);
         }

         const ZipVec& zips() const
         {
            return zipVec_;
         }
         std::shared_ptr<const Zip> zip(const std::string& id) const
         {
            ZipMap::const_iterator it = zipMap_.find(id);
            return (it == zipMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Zip> zip(const std::string& id)
         {
            return std::const_pointer_cast<Zip>((const_cast<const Network*>(this))->zip(id));
         }
         virtual void addZip(std::shared_ptr<Zip> zip, Bus& toBus)
         {
            zipMap_[zip->id()] = zip;
            toBus.addZip(zip);
            zipVec_.push_back(zip);
         }

      /// @}
         
      /// @name Per-Unit Conversions:
      /// @{
         
         template<typename T> T V2Pu(const T& V, double VBase) const
         {
            return V/VBase;
         }
         template<typename T> T pu2V(const T& pu, double VBase) const
         {
            return pu * VBase;
         }
         
         template<typename T> T S2Pu(const T& S) const
         {
            return S/PBase_;
         }
         template<typename T> T pu2S(const T& pu) const
         {
            return pu * PBase_;
         }
 
         template<typename T> T Y2Pu(const T& Y, double VBase) const
         {
            return Y * VBase * VBase/PBase_;
         }
         template<typename T> T pu2Y(const T& pu, double VBase) const
         {
            return pu * PBase_/(VBase * VBase);
         }
  
         template<typename T> T Z2Pu(const T& Z, double VBase) const
         {
            return Z * PBase_/(VBase * VBase);
         }
         template<typename T> T pu2Z(const T& pu, double VBase) const
         {
            return pu * VBase * VBase/PBase_;
         }
  
         template<typename T> T I2Pu(const T& I, double VBase) const
         {
            return I * VBase/PBase_;
         }
         template<typename T> T pu2I(const T& pu, double VBase) const
         {
            return pu * PBase_/VBase;
         }
         
      /// @}
         
      /// @name Power-Flow problem:
      /// @{
         
         virtual void solvePowerFlow();
         
      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:
         double PBase_ = 1.0;
         double freq_ = 50.0;

         BusVec busVec_;
         BusMap busMap_;

         BranchVec branchVec_;
         BranchMap branchMap_;

         GenVec genVec_;
         GenMap genMap_;

         ZipVec zipVec_;
         ZipMap zipMap_;
   };
}

#endif // NETWORK_DOT_H
