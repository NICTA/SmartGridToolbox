#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

namespace Sgt
{
   /// @brief Utility base class for a component that updates with a regular "tick" dt.
   class HeartbeatAdaptor : public SimComponentAdaptor
   {
      /// @name Lifecycle:
      /// @{

      public:

         HeartbeatAdaptor(const Time& dt) :
            dt_(dt)
         {
            // Empty.
         }

      /// @}

      /// @name Overridden member functions from SimComponent.
      /// @{

      public:

         virtual Time validUntil() const override
         {
            return nextBeat_;
         }

      protected:

         virtual void initializeState() override
         {
            nextBeat_ = posix_time::not_a_date_time;
            needsUpdate().trigger(); // Update on the first timestep.
         }

         virtual void updateState(Time t) override
         {
            if (nextBeat_ == posix_time::not_a_date_time)
            {
               nextBeat_ = t; // OK because I'm guaranteed to update on first timestep.
            }

            if (t == nextBeat_)
            {
               nextBeat_ += dt_;
            };
         }

      /// @}

      /// @name Heartbeat specific member functions.
      /// @{

      public:

         Time dt() const
         {
            return dt_;
         }

         void setDt(Time dt)
         {
            dt_ = dt;
         }

      /// @}

      private:
         Time dt_;

         Time nextBeat_{posix_time::not_a_date_time};
   };

   class Heartbeat : public HeartbeatAdaptor, public Component
   {
      public:

      /// @name Static member functions:
      /// @{

         static const std::string& sComponentType()
         {
            static std::string result("heartbeat");
            return result;
         }

      /// @}

      /// @name Lifecycle:
      /// @{

         Heartbeat(const std::string& id, const Time& dt) :
            HeartbeatAdaptor(dt),
            Component(id)
         {
            // Empty.
         }

      /// @}

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
