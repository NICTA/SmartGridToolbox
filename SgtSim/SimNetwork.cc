#include "SimNetwork.h"

#include "SimBus.h"
#include "SimBranch.h"

#include <SgtCore/Model.h>
#include <SgtCore/PowerFlowNr.h>

#include <iostream>

namespace SmartGridToolbox
{
   void SimNetwork::updateState(Time t)
   {
      SGT_DEBUG(debug() << "SimNetwork : update state." << std::endl);
      solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }

   void SimNetwork::addBus(SimBus& bus)
   {
      dependsOn(bus);
      busVec_.push_back(&bus);
      busMap_[bus.name()] = &bus;
      bus.changed().addAction([this](){needsUpdate().trigger();},
            "Trigger SimNetwork " + name() + " needs update");
   }

   void SimNetwork::addBranch(SimBranch& branch)
   {
      dependsOn(branch);
      branchVec_.push_back(&branch);
      branch.changed().addAction([this](){needsUpdate().trigger();},
            "Trigger SimNetwork " + name() + " needs update");
   }
}
