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

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("sim_network");
            return result;
         }
      
      /// @}

      /// @name Lifecycle.
      /// @{
         
         SimNetwork(std::shared_ptr<Network> network) : SimComponent(network->id()), network_(network) {}

      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; TODO

      /// @}

      /// @name Network access.
      /// @{
         
         std::shared_ptr<const Network> network() const
         {
            return network_;
         }
         
         std::shared_ptr<Network> network()
         {
            return network_;
         }

      /// @}
 
      /// @name Adding components.
      /// @{
      
      public:

         template<typename T> void addNode(std::shared_ptr<T> simBus)
         {
            network_->addNode(simBus);
            dependsOn(simBus);
            simBus->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simBus->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
         }

         template<typename T> void addArc(std::shared_ptr<T> simBranch, const std::string& bus0Id,
                             const std::string& bus1Id)
         {
            network_->addArc(simBranch, bus0Id, bus1Id);
            dependsOn(simBranch);
            simBranch->admittanceChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simBranch->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
         }

         template<typename T> void addGen(std::shared_ptr<T> simGen, const std::string& busId)
         {
            network_->addGen(simGen, busId);
            dependsOn(simGen);
            simGen->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simGen->generationChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simGen->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
         }

         template<typename T> void addZip(std::shared_ptr<T> simZip, const std::string& busId)
         {
            network_->addZip(simZip, busId);
            dependsOn(simZip);
            simZip->injectionChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simZip->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            simZip->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
                  std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
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
