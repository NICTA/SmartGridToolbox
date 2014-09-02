#include "InverterAbc.h"

#include "DcPowerSourceAbc.h"

namespace SmartGridToolbox
{
   void InverterAbc::addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source)
   {
      dependsOn(source);
      sources_.push_back(source);
      source->didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger InverterAbc " + id() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   double InverterAbc::PPerPhase() const
   {
      return PDc_ * efficiency(PDc_) / phases().size();
   }
}
