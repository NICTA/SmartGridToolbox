#ifndef REGULAR_UPDATE_COMPONENT
#define REGULAR_UPDATE_COMPONENT

#include "Component.h"

namespace SmartGridToolbox
{
   class RegularUpdateComponent : public Component
   {
      public:
         RegularUpdateComponent(time_duration dt) : dt_(dt)
         {
            // Empty.
         }

         virtual ptime getValidUntil() const
         {
            return getTime() + dt_;
         }

      private:
         time_duration dt_;
   };
}

#endif // REGULAR_UPDATE_COMPONENT
