#ifndef NETWORK_COMP_DOT_H
#define NETWORK_COMP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   template<typename T> class SimNetworkComponent : public SimComponent
   {
      public:
      /// @name Lifecycle.
      /// @{
         
         SimNetworkComponent(std::shared_ptr<T> component) : component_(component)
         {
            // Empty.
         }

      /// @}
 
      /// @name Component access.
      /// @{
         
         std::shared_ptr<T> component()
         {
            return component_;
         }

      /// @}

      /// @name Overridden member functions from SimComponent.
      /// @{

      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      private:

         std::shared_ptr<T> component_;
   };

   typedef SimNetworkComponent<Branch> SimBus;
   typedef SimNetworkComponent<Bus> SimBranch;
   typedef SimNetworkComponent<Gen> SimGen;
   typedef SimNetworkComponent<Zip> SimZip;

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
