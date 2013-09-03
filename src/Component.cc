#include "Component.h"

namespace SmartGridToolbox
{
   void Component::initialize(Time t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " initialize to " << t << std::endl);
      willUpdate_.trigger();
      startTime_ = t;
      initializeState(t);
      currentTime_ = t;
      didUpdate_.trigger();
   }

   void Component::update(Time t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " update from " << currentTime_ << " to " << t << std::endl);
      willUpdate_.trigger();
      updateState(currentTime_, t);
      currentTime_ = t;
      didUpdate_.trigger();
   }
}
