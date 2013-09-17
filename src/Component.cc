#include "Component.h"

namespace SmartGridToolbox
{
   Component::Component(const std::string & name) : 
      name_(name),
      currentTime_(not_a_date_time),
      rank_(-1),
      willUpdate_("Component " + name_ + " will update"),
      didUpdate_("Component " + name_ + " did update"),
      needsUpdate_("Component " + name_ + " needs update"),
      willStartNewTimestep_("Component " + name_ + " will start new timestep"),
      didCompleteTimestep_("Component " + name_ + " did complete timestep")
   {
      // Empty.
   }

   Component::~Component()
   {
      for (auto it : propertyMap_)
      {
         delete it.second;
      }
   }

   void Component::initialize(Time t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " initialize to " << t << std::endl);
      startTime_ = t;
      currentTime_ = neg_infin;
      initializeState();
   }

   void Component::update(Time t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " update from " << currentTime_ << " to " << t << std::endl);
      if (currentTime_ < t)
      {
         willStartNewTimestep_.trigger();
      }
      willUpdate_.trigger();
      updateState(currentTime_, t);
      currentTime_ = t;
      didUpdate_.trigger();
   }

   void Component::ensureAtTime(Time t)
   {
      if (currentTime_ == not_a_date_time)
      {
         initialize(t);
      }
      if (t > time())
      {
         update(t);
      }
      else if (t < time())
      {
         error() << "Component " << name() << " was asked to go back in time." << std::endl;
         abort();
      }
   }
}
