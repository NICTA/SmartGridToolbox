#include "InverterBase.h"

#include "DCPowerSourceBase.h"

namespace SmartGridToolbox
{
   void InverterBase::addDCPowerSource(DCPowerSourceBase & source)
   {
      sources_.push_back(&source);
      source.eventDidUpdate().addAction([this](){eventNeedsUpdate().trigger();}, 
            "Trigger InverterBase " + name() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   void InverterBase::updateState(time_duration t0, time_duration t1)
   {
      PDC_ = 0.0;
      for (const DCPowerSourceBase * source : sources_)
      {
         PDC_ += source->PDC();
      }
   }

   UblasVector<Complex> InverterBase::S() const
   {
      return UblasVector<Complex>(phases().size(), polar(PDC_ * efficiency(PDC_), phaseAngleRadians(PDC_)));
   }
}
