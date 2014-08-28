#include "InverterBase.h"

#include "DcPowerSourceBase.h"

namespace SmartGridToolbox
{
   void InverterBase::updateState(Time t)
   {
      PDc_ = 0.0;
      for (auto source : sources_)
      {
         PDc_ += source->PDc();
      }
   }

   void InverterBase::addDcPowerSource(std::shared_ptr<DcPowerSourceBase> source)
   {
      dependsOn(source);
      sources_.push_back(source);
      source->didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger InverterBase " + id() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   double InverterBase::PPerPhase() const
   {
      return PDc_ * efficiency(PDc_) / phases().size();
   }
}
