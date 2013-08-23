#include "InverterBase.h"

#include "DCPowerSourceBase.h"

namespace SmartGridToolbox
{
   void InverterBase::addDCPowerSource(DCPowerSourceBase & source)
   {
      sources_.push_back(&source);
      source.getEventDidUpdate().addAction([this](){getEventNeedsUpdate().trigger();}, 
            "Trigger InverterBase " + getName() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   void InverterBase::updateState(ptime t0, ptime t1)
   {
      PDC_ = 0.0;
      for (const DCPowerSourceBase * source : sources_)
      {
         PDC_ += source->getPDC();
      }
   }

   UblasVector<Complex> InverterBase::getS() const
   {
      return UblasVector<Complex>(getPhases().size(), polar(PDC_ * getEfficiency(PDC_), getPhaseAngleRadians(PDC_)));
   }
}
