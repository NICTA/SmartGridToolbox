#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include<SgtCore/Branch.h>
#include<SgtCore/Bus.h>
#include<SgtCore/Gen.h>
#include<SgtCore/Zip.h>

#include<iostream>
#include<map>
#include<vector>

namespace SmartGridToolbox
{
   class Network
   {
      public:

         typedef std::vector<std::unique_ptr<Bus>> BusVec;
         typedef std::vector<std::unique_ptr<Branch>> BranchVec;
         typedef std::vector<std::unique_ptr<Gen>> GenVec;
         typedef std::vector<std::unique_ptr<Zip>> ZipVec;

         typedef std::map<std::string, Bus*> BusMap;
         typedef std::map<std::string, Branch*> BranchMap;
         typedef std::map<std::string, Gen*> GenMap;
         typedef std::map<std::string, Zip*> ZipMap;

         Network(double PBase) : PBase_(PBase)
         {
            // Empty.
         }

         virtual ~Network() = default;

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

         const BusVec& busses() const
         {
            return busVec_;
         }
         const Bus* bus(const std::string& id) const
         {
            auto it = busMap_.find(id);
            return it == busMap_.end() ? 0 : it->second;
         } 
         virtual Bus* bus(const std::string& id)
         {
            return const_cast<Bus*>((const_cast<const Network*>(this))->bus(id));
         } 
         virtual void addBus(std::unique_ptr<Bus> bus)
         {
            busMap_[bus->id()] = bus.get();
            busVec_.push_back(std::move(bus));
         }

         const BranchVec& branches() const
         {
            return branchVec_;
         }
         const Branch* branch(const std::string& id) const
         {
            auto it = branchMap_.find(id);
            return it == branchMap_.end() ? 0 : it->second;
         } 
         virtual Branch* branch(const std::string& id)
         {
            return const_cast<Branch*>((const_cast<const Network*>(this))->branch(id));
         } 
         virtual void addBranch(std::unique_ptr<Branch> branch)
         {
            branchMap_[branch->id()] = branch.get();
            branchVec_.push_back(std::move(branch));
         }

         const GenVec& gens() const
         {
            return genVec_;
         }
         const Gen* gen(const std::string& id) const
         {
            auto it = genMap_.find(id);
            return it == genMap_.end() ? 0 : it->second;
         } 
         virtual Gen* gen(const std::string& id)
         {
            return const_cast<Gen*>((const_cast<const Network*>(this))->gen(id));
         } 
         virtual void addGen(std::unique_ptr<Gen> gen, Bus& toBus)
         {
            toBus.addGen(*gen);
            genMap_[gen->id()] = gen.get();
            genVec_.push_back(std::move(gen));
         }

         const ZipVec& zips() const
         {
            return zipVec_;
         }
         const Zip* zip(const std::string& id) const
         {
            auto it = zipMap_.find(id);
            return it == zipMap_.end() ? 0 : it->second;
         } 
         virtual Zip* zip(const std::string& id)
         {
            return const_cast<Zip*>((const_cast<const Network*>(this))->zip(id));
         } 
         virtual void addZip(std::unique_ptr<Zip> zip, Bus& toBus)
         {
            toBus.addZip(*zip);
            zipMap_[zip->id()] = zip.get();
            zipVec_.push_back(std::move(zip));
         }

         template<typename T> T V2Pu(const T& V, double VBase) const
         {
            return V/VBase;
         }
         template<typename T> T pu2V(const T& pu, double VBase) const
         {
            return pu*VBase;
         }
         
         template<typename T> T S2Pu(const T& S) const
         {
            return S/PBase_;
         }
         template<typename T> T pu2S(const T& pu) const
         {
            return pu*PBase_;
         }
 
         template<typename T> T Y2Pu(const T& Y, double VBase) const
         {
            return Y*VBase*VBase/PBase_;
         }
         template<typename T> T pu2Y(const T& pu, double VBase) const
         {
            return pu*PBase_/(VBase*VBase);
         }
  
         template<typename T> T Z2Pu(const T& Z, double VBase) const
         {
            return Z*PBase_/(VBase*VBase);
         }
         template<typename T> T pu2Z(const T& pu, double VBase) const
         {
            return pu*VBase*VBase/PBase_;
         }
  
         template<typename T> T I2Pu(const T& I, double VBase) const
         {
            return I*VBase/PBase_;
         }
         template<typename T> T pu2I(const T& pu, double VBase) const
         {
            return pu*PBase_/VBase;
         }
         
         friend std::ostream& operator<<(std::ostream& os, const Network& netw)
         {
            return netw.print(os);
         }
      
      protected:

         virtual std::ostream& print(std::ostream& os) const;
      
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
