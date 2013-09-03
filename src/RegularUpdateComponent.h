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

         virtual time_duration validUntil() const
         {
            return time() + dt_;
         }

         time_duration dt() const
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
