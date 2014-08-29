#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   /// @ingroup PowerFlowCore
   class SimNetwork : public Network, public SimComponent
   {
      public:

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(const std::string& id, double PBase) : Network(id, PBase) {}

      /// @}
      
      /// @name Overridden member functions from Network.
      /// @{
      
      public:

         virtual void addNode(std::shared_ptr<Bus> bus) override;
         virtual void addArc(std::shared_ptr<Branch> branch, const std::string& bus0Id,
                             const std::string& bus1Id) override;
         virtual void addGen(std::shared_ptr<Gen> gen, const std::string& busId) override;
         virtual void addZip(std::shared_ptr<Zip> zip, const std::string& busId) override;

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
