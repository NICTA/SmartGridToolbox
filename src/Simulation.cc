#include "Simulation.h"
#include "Model.h"
#include "Component.h"

namespace SmartGridToolbox
{
   void Simulation::initialize(const ptime & startTime, const ptime & endTime)
   {
      startTime_ = startTime;
      endTime_ = endTime;
      for (Model::ComponentVec::iterator it = mod_->getComponents().begin();
            it != mod_->getComponents().end(); ++it)
      {
         (**it).initializeComponent(startTime);
      }
   }

   void Simulation::advanceToNextTime()
   {
      for (Model::ComponentVec::iterator it = mod_->getComponents().begin();
            it != mod_->getComponents().end(); ++it)
      {
         (**it).advanceComponent(nextTime_);
      }
   }
}
