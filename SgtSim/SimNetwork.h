#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   /// @ingroup PowerFlowCore
   /// @brief SimNetwork : A SimComponent for an electrical network.
   class SimNetwork : public SimNetworkComponent<Network>
   {
      public:

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(std::shared_ptr<Network> network) : SimNetworkComponent<Network>(network) {}

      /// @}
 
      /// @name Adding components.
      /// @{
      
      public:

         virtual void addNode(std::shared_ptr<SimBus> bus);

         virtual void addArc(std::shared_ptr<SimBranch> branch, const std::string& bus0Id,
                             const std::string& bus1Id);

         virtual void addGen(std::shared_ptr<SimGen> gen, const std::string& busId);

         virtual void addZip(std::shared_ptr<SimZip> zip, const std::string& busId);
      
      /// @}

      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:

         // virtual Time validUntil() const override;

      protected:

         // virtual void initializeState() override;
         virtual void updateState(Time t) override;

      /// @}
      
      private:

         std::shared_ptr<Network> network_;
   };
}

#endif // NETWORK_COMP_DOT_H
