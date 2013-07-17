#ifndef NETWORK_1P_COMPONENT_DOT_H
#define NETWORK_1P_COMPONENT_DOT_H

#include "Component.h"
#include "BalancedPowerFlowNR.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class Bus1PComponent;
   class Branch1PComponent;

   class Network1PComponentParser : public ComponentParser
   {
      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, 
                                Model & mod) const override;

         static constexpr const char * getComponentName() 
         {
            return "network_1_phase";
         }
   };

   class Network1PComponent : public Component
   {
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
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         // TODO: should this be done with Events interface?
         virtual void networkHasChanged()
         {

         }
      /// @}

      /// @name My private member functions.
      /// @{
      private:
         virtual void rebuildNetwork();
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         std::vector<Bus1PComponent *> busses_;
         std::vector<Branch1PComponent *> branches_;
         BalancedPowerFlowNR solver_;
      /// @}
   };
}

#endif // NETWORK_1P_COMPONENT_DOT_H

