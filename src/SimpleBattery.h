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
         SimpleBattery(const std::string & name) :
            Component(name),
            initCharge_(0.0),
            maxChargePower_(0.0),
            maxDischargePower_(0.0),
            chargeEfficiency_(0.0),
            dischargeEfficiency_(0.0),
            charge_(0.0),
            requestedPower_(0.0)
         {
            // Empty.
         }

         double initCharge() {return initCharge_;}
         void setInitCharge(double val) {initCharge_ = val;}

         double maxCharge() {return maxCharge_;}
         void setMaxCharge(double val) {maxCharge_ = val;}

         double maxChargePower() {return maxChargePower_;}
         void setMaxChargePower(double val) {maxChargePower_ = val;}

         double maxDischargePower() {return maxDischargePower_;}
         void setMaxDischargePower(double val) {maxDischargePower_ = val;}

         double chargeEfficiency() {return chargeEfficiency_;}
         void setChargeEfficiency(double val) {chargeEfficiency_ = val;}

         double dischargeEfficiency() {return dischargeEfficiency_;}
         void setDischargeEfficiency(double val) {dischargeEfficiency_ = val;}

         double charge() {return charge_;}

         double requestedPower() {return requestedPower_;}
         void setRequestedPower(double val) {requestedPower_ = val;}

         double power() 
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_)
            : std::min(requestedPower_, maxChargePower_);
         }

         double internalPower() 
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_) / 
              dischargeEfficiency_
            : std::min(requestedPower_, maxChargePower_) * chargeEfficiency_;
         }

      private:
         virtual void initializeState(ptime t) override
         {
            charge_ = initCharge_;
         }

         virtual void updateState(ptime t0, ptime t1) override;

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
