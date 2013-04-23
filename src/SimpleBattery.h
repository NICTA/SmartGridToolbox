#ifndef SIMPLE_BATTERY_DOT_H
#define SIMPLE_BATTERY_DOT_H

#include "SimpleBattery.h"
#include "Common.h"
#include "Component.h"
#include<string>

namespace SmartGridToolbox
{
   class SimpleBattery : public Component
   {
      public:
         SimpleBattery(const std::string & name):
            Component(name),
            initCharge_(0.0),
            maxChargePower_(0.0),
            maxDischargePower_(00),
            chargeEfficiency_(0.0),
            dischargeEfficiency_(0.0),
            charge_(0.0),
            requestedPower_(0.0)
         {
            addProperty<int>(std::string("requestedPower"), 
                  [this]()->int{return requestedPower_;});
         }

      private:
         virtual void initialize() override
         {
            charge_ = initCharge_;
         }

         virtual void advance(ptime t) override;

      private:
         // Parameters.
         double initCharge_;
         double maxCharge_;
         double maxChargePower_;
         double maxDischargePower_;
         double chargeEfficiency_;
         double dischargeEfficiency_;
                  
         // State.
         double charge_;
         double requestedPower_;
   };
}
#endif // SIMPLE_BATTERY_DOT_H
