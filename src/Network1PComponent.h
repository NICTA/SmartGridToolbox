#ifndef NETWORK_1P_COMPONENT_DOT_H
#define NETWORK_1P_COMPONENT_DOT_H

#include "Component.h"
#include "BalancedPowerFlowNR.h"
#include "Bus1PComponent.h"
#include "Branch1PComponent.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class Bus1PComponent;
   class Branch1PComponent;

   class Network1PComponentParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName() 
         {
            return "network_1_phase";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override {}
   };

   class Network1PComponent : public Component
   {
      private:
         typedef std::vector<Bus1PComponent *> BusVec;
         typedef std::map<std::string, Bus1PComponent *> BusMap;
         typedef std::vector<Branch1PComponent *> BranchVec;

      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override
         {
            rebuildNetwork();
         }

         virtual void updateState(ptime t0, ptime t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:

         void addBus(Bus1PComponent & bus)
         {
            busVec_.push_back(&bus);
            busMap_[bus.getName()] = &bus;
         }

         const Bus1PComponent * findBus(const std::string & name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         Bus1PComponent * findBus(const std::string & name)
         {
            return const_cast<Bus1PComponent *>((const_cast<const Network1PComponent *>(this))->findBus(name));
         }

         void addBranch(Branch1PComponent & branch)
         {
            branchVec_.push_back(&branch);
         }

         virtual void rebuildNetwork();
      /// @}

      /// @name My private member functions.
      /// @{
      private:
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         BusVec busVec_;
         BusMap busMap_;
         BranchVec branchVec_;
         BalancedPowerFlowNR solver_;
      /// @}
   };
}

#endif // NETWORK_1P_COMPONENT_DOT_H

