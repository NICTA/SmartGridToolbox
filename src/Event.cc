#include "Common.h"
#include "Event.h"

namespace SmartGridToolbox
{
   void Event::trigger()
   {
      SGTDebug("Event was triggered: " << description_); 
      for (const Action & action : actions_)
      {
         SGTDebug("Event perform action: " << action.getDescription());
         action.perform();
         SGTDebug("Event performed action: " << action.getDescription());
      }
      SGTDebug("Event finished: " << description_); 
   }
}
