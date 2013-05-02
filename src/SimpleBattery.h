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
            addReadProperty<double>(std::string("charge"), 
                  [this](){return getCharge();});
            addReadWriteProperty<double>(std::string("requestedPower"), 
                  [this](){return getRequestedPower();},
                  [this](const double & p){setRequestedPower(p);});
         }

         double getInitCharge() {return initCharge_;}
         void setInitCharge(double val) {initCharge_ = val;}

         double getMaxCharge() {return maxCharge_;}
         void setMaxCharge(double val) {maxCharge_ = val;}

         double getMaxChargePower() {return maxChargePower_;}
         void setMaxChargePower(double val) {maxChargePower_ = val;}

         double getMaxDischargePower() {return maxDischargePower_;}
         void setMaxDischargePower(double val) {maxDischargePower_ = val;}

         double getChargeEfficiency() {return chargeEfficiency_;}
         void setChargeEfficiency(double val) {chargeEfficiency_ = val;}

         double getDischargeEfficiency() {return dischargeEfficiency_;}
         void setDischargeEfficiency(double val) {dischargeEfficiency_ = val;}

         double getCharge() {return charge_;}

         double getRequestedPower() {return requestedPower_;}
         void setRequestedPower(double val) {requestedPower_ = val;}

         double getPower() 
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_)
            : std::min(requestedPower_, maxChargePower_);
         }

         double getInternalPower() 
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_) / 
              dischargeEfficiency_
            : std::min(requestedPower_, maxChargePower_) * chargeEfficiency_;
         }

      private:
         virtual void initialize() override
         {
            charge_ = initCharge_;
         }

         virtual void updateState(ptime t) override;

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
