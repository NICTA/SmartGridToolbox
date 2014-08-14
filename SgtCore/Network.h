#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include<SgtCore/Branch.h>
#include<SgtCore/Bus.h>
#include<SgtCore/Gen.h>
#include<SgtCore/Model.h>
#include<SgtCore/Zip.h>

#include<iostream>
#include<vector>

namespace SmartGridToolbox
{
   class Network
   {
      public:

         typedef std::vector<Bus*> BusVec;
         typedef std::vector<Branch*> BranchVec;
         typedef std::vector<Gen*> GenVec;
         typedef std::vector<Zip*> ZipVec;

         Network(Model& model, double PBase) : model_(&model), PBase_(PBase)
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
            return model_->component<Bus>(id);
         } 
         virtual Bus* bus(const std::string& id)
         {
            return const_cast<Bus*>((const_cast<const Network*>(this))->bus(id));
         } 
         virtual void addBus(std::unique_ptr<Bus> bus)
         {
            busVec_.push_back(bus.get());
            model_->acquireComponent(std::move(bus));
         }

         const BranchVec& branches() const
         {
            return branchVec_;
         }
         const Branch* branch(const std::string& id) const
         {
            return model_->component<Branch>(id);
         } 
         virtual Branch* branch(const std::string& id)
         {
            return const_cast<Branch*>((const_cast<const Network*>(this))->branch(id));
         } 
         virtual void addBranch(std::unique_ptr<Branch> branch)
         {
            branchVec_.push_back(branch.get());
            model_->acquireComponent(std::move(branch));
         }

         const GenVec& gens() const
         {
            return genVec_;
         }
         const Gen* gen(const std::string& id) const
         {
            return model_->component<Gen>(id);
         } 
         virtual Gen* gen(const std::string& id)
         {
            return const_cast<Gen*>((const_cast<const Network*>(this))->gen(id));
         } 
         virtual void addGen(std::unique_ptr<Gen> gen, Bus& toBus)
         {
            toBus.addGen(*gen);
            genVec_.push_back(gen.get());
            model_->acquireComponent(std::move(gen));
         }

         const ZipVec& zips() const
         {
            return zipVec_;
         }
         const Zip* zip(const std::string& id) const
         {
            return model_->component<Zip>(id);
         } 
         virtual Zip* zip(const std::string& id)
         {
            return const_cast<Zip*>((const_cast<const Network*>(this))->zip(id));
         } 
         virtual void addZip(std::unique_ptr<Zip> zip, Bus& toBus)
         {
            toBus.addZip(*zip);
            zipVec_.push_back(zip.get());
            model_->acquireComponent(std::move(zip));
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
         Model* model_;

         double PBase_ = 1.0;
         double freq_ = 50.0;

         BusVec busVec_;
         BranchVec branchVec_;
         GenVec genVec_;
         ZipVec zipVec_;
   };
}

#endif // NETWORK_DOT_H
