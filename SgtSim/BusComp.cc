#include "BusComp.h"

#include "Model.h"
#include "NetworkComp.h"
#include "ZipBase.h"

namespace SmartGridToolbox
{
   BusComp::BusComp(const std::string& name, Phases phases, const ublas::vector<Complex> & VNom, double VBase) :
      Component(name),
      Bus(name, phases, VNom, VBase),
      changed_("BusComp " + name + " setpoint changed")
   {
      // Empty.
   }

   void BusComp::addZip(ZipBase& zip)
   {
      dependsOn(zip);
      zips_.push_back(&zip);
      zip.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger BusComp " + name() + " needsUpdate.");
      zip.didUpdate().addAction([this](){changed().trigger();},
            "Trigger BusComp " + name() + " changed.");
      // TODO: this will recalculate all zips. Efficiency?
      changed().trigger();
   }
}
