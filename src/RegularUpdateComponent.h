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

         virtual Time validUntil() const
         {
            return time() + dt_;
         }

         Time dt() const
         {
            return dt_;
         }
         void setDt(Time dt)
         {
            dt_ = dt;
         }

      private:
         Time dt_;
   };
}

#endif // REGULAR_UPDATE_COMPONENT
