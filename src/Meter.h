#ifndef METER_DOT_H
#define METER_DOT_H

#include "Component.h"
#include "LoadAbc.h"
#include <vector>

namespace SmartGridToolbox
{
   class Meter : public Component, public Load
   {
      // Overridden functions: from Component.
      public:
         virtual ptime getValidUntil() const override {return getTime() + dt_;}

      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;

      // Overridden functions: from LoadAbc.
      public:
         virtual Complex getPower() override {return Complex(Ph_, 0.0);}

      public:
         Meter()

         void addLoadComponent(const Component & comp)
         {
            comps_.push_back(&comp);
         }

      private:
         time_duration dt_;
         std::vector<Component *> comps_;
   };
}

#endif // METER_DOT_H
