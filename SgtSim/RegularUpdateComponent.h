#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

namespace SmartGridToolbox
{
   /// @brief Utility base class for a component that updates with a regular "tick" dt.
   class RegularUpdateComponentAdaptor : public SimComponentAdaptor
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         virtual Time validUntil() const override
         {
            return lastUpdated() + dt_; // TODO: what if an update happened that wasn't a "tick"?
         }

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}

      /// @name My public member functions.
      /// @{
      
      public:
         RegularUpdateComponentAdaptor(const Time& dt) : dt_(dt)
         {
            needsUpdate().trigger(); // First update is at the start.
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

   class RegularUpdateComponent : public RegularUpdateComponentAdaptor, public Component
   {
      public:

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("component");
            return result;
         }
      
      /// @}
         
         RegularUpdateComponent(const std::string& id, const Time& dt) : 
            RegularUpdateComponentAdaptor(dt),
            Component(id)
         {
            // Empty.
         }

      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; TODO

      /// @}
   };
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
