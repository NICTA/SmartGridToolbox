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

   class NetworkParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey() 
         {
            return "network";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
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
         virtual void updateState(Time t0, Time t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         Network(const std::string & name, double freq) : Component(name), freq_(freq) {}

         double freq() const {return freq_;}

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

         virtual void solvePowerFlow();

         void print();
      /// @}

      /// @name My private member functions.
      /// @{
      private:
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         double freq_;
         BusVec busVec_;
         BusMap busMap_;
         BranchVec branchVec_;
         PowerFlowNR solver_;
      /// @}
   };
}

#endif // NETWORK_DOT_H

