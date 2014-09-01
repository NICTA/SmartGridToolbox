#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SgtSim/Component.h>

namespace SmartGridToolbox
{
   /// @brief Utility base class for a component that updates with a regular "tick" dt.
   class RegularUpdateComponent : public Component
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         virtual Time validUntil() const override
         {
            return time() + dt_;
         }

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}

      /// @name My public member functions.
      /// @{
      
      public:
         RegularUpdateComponent(const std::string& id) : Component(id), dt_(posix_time::seconds(0))
         {
            // Empty.
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

      /// @}
      
      private:
         Time dt_;
   };
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
