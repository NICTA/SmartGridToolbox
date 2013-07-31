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
         error() << "Component " << getName() << " can't go back in time." << std::endl;
         abort();
      }
   }
}
