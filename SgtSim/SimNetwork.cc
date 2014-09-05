#include "SimNetwork.h"
#include "SimNetworkComponent.h"


namespace SmartGridToolbox
{
   void SimNetwork::addNode(std::shared_ptr<SimBus> simBus)
   {
      auto bus = simBus->component();
      network_->addNode(bus);
      dependsOn(simBus);

      bus->controlChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      bus->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addArc(std::shared_ptr<SimBranchAbc> simBranch, const std::string& bus0Id,
         const std::string& bus1Id)
   {
      auto branch = simBranch->component();
      network_->addArc(branch, bus0Id, bus1Id);
      dependsOn(simBranch);

      branch->admittanceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      branch->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addGen(std::shared_ptr<SimGenAbc> simGen, const std::string& busId)
   {
      auto gen = simGen->component();
      network_->addGen(gen, busId);
      dependsOn(simGen);

      gen->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->generationChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addZip(std::shared_ptr<SimZipAbc> simZip, const std::string& busId)
   {
      auto zip = simZip->component();
      network_->addZip(zip, busId);
      dependsOn(simZip);

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
