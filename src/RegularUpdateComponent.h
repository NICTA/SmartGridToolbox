#ifndef REGULAR_UPDATE_COMPONENT
#define REGULAR_UPDATE_COMPONENT

#include "Component.h"

namespace SmartGridToolbox
{
   class RegularUpdateComponent : public Component
   {
      public:
         RegularUpdateComponent() : dt_(seconds(0))
         {
            // Empty.
         }

         time_duration getDt() const
         {
            return dt_;
         }
         void setDt(time_duration dt)
         {
            dt_ = dt;
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
