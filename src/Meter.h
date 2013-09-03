#ifndef METER_DOT_H
#define METER_DOT_H

#include "Component.h"
#include "Load.h"
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   class Meter : public Component, public Load
   {
      // Overridden functions: from Component.
      public:
         virtual time_duration validUntil() const override {return time() + dt_;}

      private:
         virtual void initializeState(time_duration t) override 
         {
            P_ = Complex(0.0, 0.0);
         }

         virtual void updateState(time_duration t0, time_duration t1) override;

      // Overridden functions: from Load.
      public:
         virtual Complex power() const override {return P_;}

      public:
         Meter(const std::string & name) :
            Component(name),
            dt_(minutes(5)),
            P_(Complex(0.0, 0.0)) 
         {
            // Empty.
         }

         void addLoadComponent(Component & comp)
         {
            comps_.push_back(&comp);
         }

      private:
         time_duration dt_;
         Complex P_;
         std::vector<Component *> comps_;
   };
}

#endif // METER_DOT_H
