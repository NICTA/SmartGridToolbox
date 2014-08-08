#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/Component.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   class BranchComp;
   class BusComp;

   /// @ingroup PowerFlowCore
   class NetworkComp : public Component
   {
      public:
         typedef std::vector<BusComp*> BusVec;
         typedef std::vector<BranchComp*> BranchVec;

      private:
         typedef std::map<std::string, BusComp*> BusMap;

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
         
         NetworkComp(const std::string& name, double freq) : Component(name), freq_(freq) {}

         double freq() const {return freq_;}

         void addBus(BusComp& bus);

         const BusVec& busVec() const {return busVec_;}

         const BusComp* findBus(const std::string& name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         BusComp* findBus(const std::string& name)
         {
            return const_cast<BusComp*>((const_cast<const NetworkComp*>(this))->findBus(name));
         }

         void addBranch(BranchComp& branch);

         const BranchVec& branchVec() const {return branchVec_;}

         virtual void solvePowerFlow();

         friend std::ostream& operator<<(std::ostream& os, const NetworkComp& nw);

      /// @}
      
      private:
         double freq_;
         BusVec busVec_;
         BusMap busMap_;
         BranchVec branchVec_;
   };
}

#endif // NETWORK_COMP_DOT_H

