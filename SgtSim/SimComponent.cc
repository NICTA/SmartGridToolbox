#include "SimComponent.h"

namespace SmartGridToolbox
{
   SimComponent::SimComponent(const std::string& id) :
      Component(id),
      willUpdate_("Will update"),
      didUpdate_("Did update"),
      needsUpdate_("Needs update"),
      willStartNewTimestep_("Will start new timestep"),
      didCompleteTimestep_("Did complete timestep")
   {
      // Empty.
   }

   void SimComponent::initialize()
   {
      SGT_DEBUG(debug() << "SimComponent " << id() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void SimComponent::update(Time t)
   {
      SGT_DEBUG(debug() << "SimComponent " << id() << " update from " << time_ << " to " << t << std::endl);
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
