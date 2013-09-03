#include "Component.h"

namespace SmartGridToolbox
{
   void Component::update(ptime t)
   {
      SGT_DEBUG(debug() << "Component " << name() << " update from " << t_ << " to " << t << std::endl);
      willUpdate_.trigger();
      updateState(t_, t);
      t_ = t;
      didUpdate_.trigger();
   }
}
