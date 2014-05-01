#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class Bus;

   /// @ingroup PowerFlowCore
   class Network : public Component
   {
      public:
         typedef std::vector<Bus*> BusVec;
         typedef std::vector<Branch*> BranchVec;

      private:
         typedef std::map<std::string, Bus*> BusMap;

      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         virtual void updateState(Time t) override;

      /// @}

      public:
      /// @name My public member functions.
      /// @{
         
         Network(const std::string& name, double freq) : Component(name), freq_(freq) {}

         double freq() const {return freq_;}

         void addBus(Bus& bus);

         const BusVec& busVec() const {return busVec_;}

         const Bus* findBus(const std::string& name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         Bus* findBus(const std::string& name)
         {
            return const_cast<Bus*>((const_cast<const Network*>(this))->findBus(name));
         }

         void addBranch(Branch& branch);

         const BranchVec& branchVec() const {return branchVec_;}

         virtual void solvePowerFlow();

         friend std::ostream& operator<<(std::ostream& os, const Network& nw);

      /// @}
      
      private:
         double freq_;
         BusVec busVec_;
         BusMap busMap_;
         BranchVec branchVec_;
   };
}

#endif // NETWORK_DOT_H

