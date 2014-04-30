#include <SmartGridToolbox/Component.h>

namespace SmartGridToolbox
{
   Component::Component(const std::string& name) :
      name_(name),
      time_(posix_time::not_a_date_time),
      rank_(-1),
      willUpdate_("Component " + name_ + " will update"),
      didUpdate_("Component " + name_ + " did update"),
      needsUpdate_("Component " + name_ + " needs update"),
      willStartNewTimestep_("Component " + name_ + " will start new timestep"),
      didCompleteTimestep_("Component " + name_ + " did complete timestep")
   {
      // Empty.
   }

   void Component::initialize()
   {
      SGT_DEBUG(debug() << "Component " << name() << " initialize." << std::endl);
      time_ = posix_time::neg_infin;
      initializeState();
   }

   void Component::update(Time t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " update from " << time_ << " to " << t << std::endl);
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
