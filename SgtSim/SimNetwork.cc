#include "SimNetwork.h"
#include "SimNetworkComponent.h"


namespace SmartGridToolbox
{
   void SimNetwork::addNode(std::shared_ptr<SimBusInterface> bus)
   {
      network_->addNode(bus);

      dependsOn(bus);

      bus->controlChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      bus->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addArc(std::shared_ptr<SimBranchInterface> branch, const std::string& bus0Id,
         const std::string& bus1Id)
   {
      network_->addArc(branch, bus0Id, bus1Id);

      dependsOn(branch);

      branch->admittanceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      branch->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addGen(std::shared_ptr<SimGenInterface> gen, const std::string& busId)
   {
      network_->addGen(gen, busId);

      dependsOn(gen);

      gen->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->generationChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addZip(std::shared_ptr<SimZipInterface> zip, const std::string& busId)
   {
      network_->addZip(zip, busId);

      dependsOn(zip);

      zip->injectionChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      zip->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      zip->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::updateState(Time t)
   {
      SGT_DEBUG(debug() << "SimNetwork : update state." << std::endl);
      network_->solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }
}
