#include "Simulation.h"
#include "Model.h"
#include "Component.h"

namespace SmartGridToolbox
{
   void Simulation::Initialize(const ptime & startTime, const ptime & endTime)
   {
      startTime_ = startTime;
      endTime_ = endTime;
      for (Model::ComponentVec::iterator it = mod_->beginComponent();
            it != mod_->endComponent(); ++it)
      {
         (**it).Initialize(startTime);
      }
   }

   void Simulation::AdvanceToNextTime()
   {
      for (Model::ComponentVec::iterator it = mod_->beginComponent();
            it != mod_->endComponent(); ++it)
      {
         (**it).AdvanceToTime(nextTime_);
      }
   }
}
