#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/Simulated.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   class SimBranch;
   class SimBus;

   /// @ingroup PowerFlowCore
   class SimNetwork : public Network, public Simulated
   {
      /// @name Overridden member functions from Simulated.
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
         
         SimNetwork(const std::string& id, double PBase) : Network(id, PBase) {}

      /// @}
   };
}

#endif // NETWORK_COMP_DOT_H

