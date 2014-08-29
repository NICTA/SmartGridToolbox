#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   class SimBus;
   class SimBranch;
   class SimGen;
   class SimZip;

   /// @ingroup PowerFlowCore
   class SimNetwork : public Network, public SimComponent
   {
      public:

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(const std::string& id, double PBase) : Network(id, PBase) {}

      /// @}
      
      /// @name Adding components. Note that these are NOT overridden from Network.
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
   };
}

#endif // NETWORK_COMP_DOT_H
