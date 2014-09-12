#include "SimComponent.h"

namespace SmartGridToolbox
{
   SimComponentAbc::SimComponentAbc() :
      willUpdate_("Will update"),
      didUpdate_("Did update"),
      needsUpdate_("Needs update"),
      willStartNewTimestep_("Will start new timestep"),
      didCompleteTimestep_("Did complete timestep")
   {
      // Empty.
   }

   void SimComponentAbc::initialize()
   {
      SGT_DEBUG(Log().debug() << "SimComponent " << id() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void SimComponentAbc::update(Time t)
   {
      SGT_DEBUG(Log().debug() << "SimComponent " << id() << " update from " << time_ << " to " << t << std::endl);
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
