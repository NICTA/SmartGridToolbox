#include "SimComponent.h"

namespace SmartGridToolbox
{
   SimComponent::SimComponent(const std::string& name) :
      name_(name),
      time_(posix_time::not_a_date_time),
      rank_(-1),
      willUpdate_("SimComponent " + name_ + " will update"),
      didUpdate_("SimComponent " + name_ + " did update"),
      needsUpdate_("SimComponent " + name_ + " needs update"),
      willStartNewTimestep_("SimComponent " + name_ + " will start new timestep"),
      didCompleteTimestep_("SimComponent " + name_ + " did complete timestep")
   {
      // Empty.
   }

   void SimComponent::initialize()
   {
      SGT_DEBUG(debug() << "SimComponent " << name() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void SimComponent::update(Time t)
   {
      SGT_DEBUG(debug() << "SimComponent " << name() << " update from " << time_ << " to " << t << std::endl);
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
