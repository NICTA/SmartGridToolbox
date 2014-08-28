#include "SimObject.h"

namespace SmartGridToolbox
{
   SimObject::SimObject() :
      willUpdate_("SimObject " + id() + " will update"),
      didUpdate_("SimObject " + id() + " did update"),
      needsUpdate_("SimObject " + id() + " needs update"),
      willStartNewTimestep_("SimObject " + id() + " will start new timestep"),
      didCompleteTimestep_("SimObject " + id() + " did complete timestep")
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
