#ifndef SIM_NETWORK_DOT_H
#define SIM_NETWORK_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   class SimBranchAbc;
   class SimBus;
   class SimGenAbc;
   class SimZipAbc;

   /// @ingroup PowerFlowCore
   /// @brief SimNetwork : A SimComponent for an electrical network.
   class SimNetwork : public SimComponent
   {
      public:

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(std::shared_ptr<Network> network) : SimComponent(network->id()), network_(network) {}

      /// @}

      /// @name Network access.
      /// @{
         
         std::shared_ptr<const Network> network()
         {
            return network_;
         }

      /// @}
 
      /// @name Adding components.
      /// @{
      
      public:

         virtual void addNode(std::shared_ptr<SimBus> bus);

         virtual void addArc(std::shared_ptr<SimBranchAbc> branch, const std::string& bus0Id,
                             const std::string& bus1Id);

         virtual void addGen(std::shared_ptr<SimGenAbc> gen, const std::string& busId);

         virtual void addZip(std::shared_ptr<SimZipAbc> zip, const std::string& busId);
      
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

#endif // SIM_NETWORK_DOT_H
