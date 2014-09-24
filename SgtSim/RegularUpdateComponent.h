#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

namespace SmartGridToolbox
{
   /// @brief Utility base class for a component that updates with a regular "tick" dt.
   class RegularUpdateComponentAbc : public SimComponentAbc
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
         RegularUpdateComponentAbc(const Time& dt) : dt_(dt)
         {
            needsUpdate().trigger(); // First update is at the start.
         }

         static constexpr const char* sComponentType()
         {
            return "regular_update_component";
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

   class RegularUpdateComponent : public RegularUpdateComponentAbc, public Component
   {
      public:
         RegularUpdateComponent(const std::string& id, const Time& dt) : 
            RegularUpdateComponentAbc(dt),
            Component(id)
         {
            // Empty.
         }
         
         virtual const char* componentType() const override
         {
            return sComponentType();
         }
   };
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
