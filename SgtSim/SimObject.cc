#include "SimObject.h"

namespace SmartGridToolbox
{
   SimObject::SimObject(const std::string& name) :
      name_(name),
      time_(posix_time::not_a_date_time),
      rank_(-1),
      willUpdate_("SimObject " + name_ + " will update"),
      didUpdate_("SimObject " + name_ + " did update"),
      needsUpdate_("SimObject " + name_ + " needs update"),
      willStartNewTimestep_("SimObject " + name_ + " will start new timestep"),
      didCompleteTimestep_("SimObject " + name_ + " did complete timestep")
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
