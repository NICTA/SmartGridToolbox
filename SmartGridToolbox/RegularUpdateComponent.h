#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SmartGridToolbox/Component.h>

namespace SmartGridToolbox
{
   /// Utility base class for a component that updates with a regular "tick" dt.
   class RegularUpdateComponent : public Component
   {
      public:
         RegularUpdateComponent(const std::string & name) : Component(name), dt_(posix_time::seconds(0))
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
            needsUpdate().trigger();
         }

      private:
         Time dt_;
   };
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
