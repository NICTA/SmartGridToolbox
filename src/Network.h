#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include "Component.h"
#include "PowerFlowNR.h"
#include "Bus.h"
#include "Branch.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class Bus;

   class NetworkParser : public ComponentParser
   {
      public:
         static constexpr const char * componentName() 
         {
            return "network";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override {}
   };

   class Network : public Component
   {
      private:
         typedef std::vector<Bus *> BusVec;
         typedef std::map<std::string, Bus *> BusMap;
         typedef std::vector<Branch *> BranchVec;

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(Time t) override
         {
            SGT_DEBUG(debug() << "Network : initialize state." << std::endl);
            updateState(t, t);
         }

         virtual void updateState(Time t0, Time t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         Network(const std::string & name) : Component(name) {}

         void addBus(Bus & bus);

         const Bus * findBus(const std::string & name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         Bus * findBus(const std::string & name)
         {
            return const_cast<Bus *>((const_cast<const Network *>(this))->findBus(name));
         }

         void addBranch(Branch & branch);

         virtual void rebuildNetwork();

         void print();
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
         PowerFlowNR solver_;
      /// @}
   };
}

#endif // NETWORK_DOT_H

