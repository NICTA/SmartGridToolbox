#ifndef SIM_NETWORK_DOT_H
#define SIM_NETWORK_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Network.h>

namespace SmartGridToolbox
{
   /// @ingroup PowerFlowCore
   /// @brief SimNetwork : A SimComponent for an electrical network.
   class SimNetwork : public SimComponent
   {
      public:

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(std::shared_ptr<Network> network) : SimComponent(network->id()), network_(network) {}

      /// @}

      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "sim_network";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

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

         template<typename T> void addNode(std::shared_ptr<T> simBus)
         {
            auto bus = simBus->bus();
            network_->addNode(bus);
            dependsOn(simBus);
            bus->controlChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            bus->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
         }

         template<typename T> void addArc(std::shared_ptr<T> simBranch, const std::string& bus0Id,
                             const std::string& bus1Id)
         {
            auto branch = simBranch->branch();
            network_->addArc(branch, bus0Id, bus1Id);
            dependsOn(simBranch);
            branch->admittanceChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            branch->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
         }

         template<typename T> void addGen(std::shared_ptr<T> simGen, const std::string& busId)
         {
            auto gen = simGen->gen();
            network_->addGen(gen, busId);
            dependsOn(simGen);
            gen->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            gen->generationChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            gen->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
         }

         template<typename T> void addZip(std::shared_ptr<T> simZip, const std::string& busId)
         {
            auto zip = simZip->zip();
            network_->addZip(zip, busId);
            dependsOn(simZip);
            zip->injectionChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            zip->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
            zip->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  "Trigger SimNetwork " + id() + " needs update");
         }

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
