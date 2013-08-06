#ifndef REGULAR_UPDATE_COMPONENT
#define REGULAR_UPDATE_COMPONENT

#include "Component.h"

namespace SmartGridToolbox
{
   class RegularUpdateComponent : public Component
   {
      public:
         RegularUpdateComponent(const std::string & name) : Component(name), dt_(seconds(0))
         {
            // Empty.
         }

         virtual ptime getValidUntil() const
         {
            return getTime() + dt_;
         }

         time_duration getDt() const
         {
            return dt_;
         }
         void setDt(time_duration dt)
         {
            dt_ = dt;
         }

      private:
         time_duration dt_;
   };
}

#endif // REGULAR_UPDATE_COMPONENT
