#include "SimObject.h"

namespace SmartGridToolbox
{
   SimObject::SimObject() :
      willUpdate_("Will update"),
      didUpdate_("Did update"),
      needsUpdate_("Needs update"),
      willStartNewTimestep_("Will start new timestep"),
      didCompleteTimestep_("Did complete timestep")
   {
      // Empty.
   }

   void SimObject::initialize()
   {
      SGT_DEBUG(debug() << "SimObject " << name() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void SimObject::update(Time t)
   {
      SGT_DEBUG(debug() << "SimObject " << name() << " update from " << time_ << " to " << t << std::endl);
      if (time_ < t)
      {
         willStartNewTimestep_.trigger();
      }
      willUpdate_.trigger();
      updateState(t);
      time_ = t;
      didUpdate_.trigger();
   }
}
