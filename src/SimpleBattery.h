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
            addProperty<int>(std::string("charge"), 
                  [this]()->int{return getCharge();});
            addProperty<int>(std::string("requestedPower"), 
                  [this]()->int{return getRequestedPower();});
         }

         double getInitCharge() {return initCharge_;}
         void setInitCharge(double val) {initCharge_ = val;}

         double getMaxCharge() {return maxCharge_;}
         void setMaxCharge(double val) {maxCharge_ = val;}

         double getMaxChargePower() {return maxChargePower_;}
         void setMaxChargePower(double val) {maxChargePower_ = val;}

         double getChargeEfficiency() {return chargeEfficiency_;}
         void setChargeEfficiency(double val) {chargeEfficiency_ = val;}

         double getDischargeEfficiency() {return dischargeEfficiency_;}
         void setDischargeEfficiency(double val) {dischargeEfficiency_ = val;}

         double getCharge() {return charge_;}
         void setCharge(double val) {charge_ = val;}

         double getRequestedPower() {return requestedPower_;}
         void setRequestedPower_(double val) {requestedPower_ = val;}

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
