#ifndef NETWORK_1P_DOT_H
#define NETWORK_1P_DOT_H

#include "Component.h"
#include "PowerFlow1PNR.h"
#include "Bus1P.h"
#include "Branch1P.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class Bus1P;
   class Branch1P;

   class Network1PParser : public ComponentParser
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

   class Network1P : public Component
   {
      private:
         typedef std::vector<Bus1P *> BusVec;
         typedef std::map<std::string, Bus1P *> BusMap;
         typedef std::vector<Branch1P *> BranchVec;

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
         Network1P(const std::string & name) : Component(name) {}

         void addBus(Bus1P & bus)
         {
            busVec_.push_back(&bus);
            busMap_[bus.getName()] = &bus;
            bus.getEventDidUpdate().addAction([&](){this->getEventNeedsUpdate().trigger();});
         }

         const Bus1P * findBus(const std::string & name) const
         {
            BusMap::const_iterator it = busMap_.find(name);
            return (it == busMap_.end()) ? 0 : it->second;
         }
         Bus1P * findBus(const std::string & name)
         {
            return const_cast<Bus1P *>((const_cast<const Network1P *>(this))->findBus(name));
         }

         void addBranch(Branch1P & branch)
         {
            branchVec_.push_back(&branch);
            branch.getEventDidUpdate().addAction([&](){this->getEventNeedsUpdate().trigger();});
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
         PowerFlow1PNR solver_;
      /// @}
   };
}

#endif // NETWORK_1P_DOT_H

