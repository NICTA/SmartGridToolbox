#include "SimNetwork.h"

#include "SimBranch.h"
#include "SimBus.h"
#include "SimGen.h"
#include "SimZip.h"

#include <SgtCore/PowerFlowNr.h>

#include <iostream>

namespace SmartGridToolbox
{
   void SimNetwork::addNode(std::shared_ptr<SimBus> bus)
   {
      Network::addNode(bus);

      dependsOn(bus);

      bus->controlChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      bus->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addArc(std::shared_ptr<SimBranch> branch, const std::string& bus0Id,
         const std::string& bus1Id)
   {
      Network::addArc(branch, bus0Id, bus1Id);

      dependsOn(branch);

      branch->admittanceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      branch->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addGen(std::shared_ptr<SimGen> gen, const std::string& busId)
   {
      Network::addGen(gen, busId);

      dependsOn(gen);

      gen->setpointChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->generationChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
      gen->isInServiceChanged().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + id() + " needs update");
   }

   void SimNetwork::addZip(std::shared_ptr<SimZip> zip, const std::string& busId)
   {
      Network::addZip(zip, busId);

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
      solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }
}
