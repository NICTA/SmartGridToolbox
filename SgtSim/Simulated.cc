#include "Simulated.h"

namespace SmartGridToolbox
{
   Simulated::Simulated(const std::string& name) :
      name_(name),
      time_(posix_time::not_a_date_time),
      rank_(-1),
      willUpdate_("Simulated " + name_ + " will update"),
      didUpdate_("Simulated " + name_ + " did update"),
      needsUpdate_("Simulated " + name_ + " needs update"),
      willStartNewTimestep_("Simulated " + name_ + " will start new timestep"),
      didCompleteTimestep_("Simulated " + name_ + " did complete timestep")
   {
      // Empty.
   }

   void Simulated::initialize()
   {
      SGT_DEBUG(debug() << "Simulated " << name() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void Simulated::update(Time t)
   {
      SGT_DEBUG(debug() << "Simulated " << name() << " update from " << time_ << " to " << t << std::endl);
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
