#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/Component.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   class SimBranch;
   class SimBus;

   /// @ingroup PowerFlowCore
   class SimNetwork : public Component
   {
      public:
         typedef std::vector<SimBus*> BusVec;
         typedef std::vector<SimBranch*> BranchVec;

      private:
         typedef std::map<std::string, SimBus*> BusMap;

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
         
         SimNetwork(const std::string& name, double freq) : Component(name), freq_(freq) {}

         double freq() const {return freq_;}

         void addBus(SimBus& bus);

         const BusVec& busVec() const {return busVec_;}

         const SimBus* findBus(const std::string& name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         SimBus* findBus(const std::string& name)
         {
            return const_cast<SimBus*>((const_cast<const SimNetwork*>(this))->findBus(name));
         }

         void addBranch(SimBranch& branch);

         const BranchVec& branchVec() const {return branchVec_;}

         virtual void solvePowerFlow();

         friend std::ostream& operator<<(std::ostream& os, const SimNetwork& nw);

      /// @}
      
      private:
         double freq_;
         BusVec busVec_;
         BusMap busMap_;
         BranchVec branchVec_;
   };
}

#endif // NETWORK_COMP_DOT_H

