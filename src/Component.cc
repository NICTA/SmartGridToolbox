#include "Component.h"

namespace SmartGridToolbox
{
   void Component::ensureAtTime(ptime t)
   {
      if (t_ < t)
      {
         update(t);
      }
      else if (t_ > t)
      {
         error() << "Component " << name() << " can't go back in time." << std::endl;
         abort();
      }
   }

   void Component::update(ptime t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " update from " << t_ << " to " << t << std::endl);
      willUpdate_.trigger();
      updateState(t_, t);
      t_ = t;
      didUpdate_.trigger();
   }
}
