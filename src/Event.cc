#include "Common.h"
#include "Event.h"

namespace SmartGridToolbox
{
   void Event::trigger()
   {
      SGT_DEBUG(debug() << "Event was triggered: " << description_ << std::endl); 
      for (const Action & action : actions_)
      {
         SGT_DEBUG(debug() << "Event perform action: " << action.getDescription() << std::endl);
         action.perform();
         SGT_DEBUG(debug() << "Event performed action: " << action.getDescription() << std::endl);
      }
      SGT_DEBUG(debug() << "Event finished: " << description_ << std::endl); 
   }
}
