#include "InverterBase.h"

#include "DCPowerSourceBase.h"

namespace SmartGridToolbox
{
   void InverterBase::addDCPowerSource(DCPowerSourceBase & source)
   {
      dependsOn(source);
      sources_.push_back(&source);
      source.didUpdate().addAction([this](){needsUpdate().trigger();}, 
            "Trigger InverterBase " + name() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   void InverterBase::updateState(Time t0, Time t1)
   {
      PDC_ = 0.0;
      for (const DCPowerSourceBase * source : sources_)
      {
         PDC_ += source->PDC();
      }
   }

   double InverterBase::PPerPhase() const
   {
      return PDC_ * efficiency(PDC_) / phases().size();
   }
}
