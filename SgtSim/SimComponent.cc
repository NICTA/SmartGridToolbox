#include "SimComponent.h"

namespace SmartGridToolbox
{
   void SimComponentAbc::initialize()
   {
      SGT_DEBUG(Log().debug() << "SimComponent " << id() << " initialize." << std::endl);
      willUpdate_.setDescription(id() + ": Will update");
      didUpdate_.setDescription(id() + ": Did update");
      needsUpdate_.setDescription(id() + ": Needs update");
      willStartNewTimestep_.setDescription(id() + ": Will start new timestep");
      didCompleteTimestep_.setDescription(id() + ": Did complete timestep");
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
