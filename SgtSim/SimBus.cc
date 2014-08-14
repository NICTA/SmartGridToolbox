#include "SimBus.h"

#include "Model.h"
#include "SimNetwork.h"
#include "ZipBase.h"

namespace SmartGridToolbox
{
   SimBus::SimBus(const std::string& name, Phases phases, const ublas::vector<Complex> & VNom, double VBase) :
      Component(name),
      Bus(name, phases, VNom, VBase),
      changed_("SimBus " + name + " setpoint changed")
   {
      // Empty.
   }

   void SimBus::addZip(ZipBase& zip)
   {
      dependsOn(zip);
      zips_.push_back(&zip);
      zip.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger SimBus " + name() + " needsUpdate.");
      zip.didUpdate().addAction([this](){changed().trigger();},
            "Trigger SimBus " + name() + " changed.");
      // TODO: this will recalculate all zips. Efficiency?
      changed().trigger();
   }
}
