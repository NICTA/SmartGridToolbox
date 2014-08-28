#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   /// @ingroup PowerFlowCore
   class SimNetwork : public Network, public SimComponent
   {
      /// @name Overridden member functions from Network.
      /// @{
      
      public:

         virtual void addBus(std::shared_ptr<Bus> bus) override;
         virtual void addBranch(std::shared_ptr<Branch> branch) override;
         virtual void addGen(std::shared_ptr<Gen> gen) override;
         virtual void addZip(std::shared_ptr<Zip> zip) override;

      /// @name Overridden member functions from SimComponent.
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
