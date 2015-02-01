#include "SimComponent.h"

namespace SmartGridToolbox
{
   void SimComponentAdaptor::initialize()
   {
      SGT_DEBUG(Log().debug() << "SimComponent " << id() << " initialize." << std::endl);
      willUpdate_.setDescription(id() + ": Will update");
      didUpdate_.setDescription(id() + ": Did update");
      needsUpdate_.setDescription(id() + ": Needs update");
      willStartNewTimestep_.setDescription(id() + ": Will start new timestep");
      didCompleteTimestep_.setDescription(id() + ": Did complete timestep");
      lastUpdated_ = posix_time::neg_infin;
      initializeState();
   }

   void SimComponentAdaptor::update(Time t)
   {
      SGT_DEBUG(Log().debug() << "SimComponent " << id() << " update from " << lastUpdated_ << " to " << t 
            << std::endl);
      if (lastUpdated_ < t)
      {
         willStartNewTimestep_.trigger();
      }
      willUpdate_.trigger();
      updateState(t);
      lastUpdated_ = t;
      didUpdate_.trigger();
   }
}
