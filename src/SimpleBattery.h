#ifndef SIMPLE_BATTERY_DOT_H
#define SIMPLE_BATTERY_DOT_H

#include <SmartGridToolbox/SimpleBattery.h>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/DcPowerSourceBase.h>
#include <SmartGridToolbox/Parser.h>
#include<string>

namespace SmartGridToolbox
{
   class SimpleBatteryParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "simple_battery";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   class SimpleBattery : public DcPowerSourceBase
   {
      // Overridden functions: from Component.
      protected:
         virtual void initializeState() override
         {
            charge_ = initCharge_;
         }

         virtual void updateState(Time t0, Time t1) override;

         
         virtual Time validUntil() const override
         {
            return time() + dt_;
         }

      // Overridden functions: from DCPowerSourceBase.
      public:
         virtual double PDc() const override
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_)
            : std::min(requestedPower_, maxChargePower_);
         }

      // Public member functions.
      public:
         SimpleBattery(const std::string & name) :
            DcPowerSourceBase(name),
            dt_(posix_time::minutes(5)),
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
         
         Time get_dt() {return dt_;}
         void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}
      
         double initCharge() {return initCharge_;}
         void setInitCharge(double val) {initCharge_ = val; needsUpdate().trigger();}

         double maxCharge() {return maxCharge_;}
         void setMaxCharge(double val) {maxCharge_ = val; needsUpdate().trigger();}

         double maxChargePower() {return maxChargePower_;}
         void setMaxChargePower(double val) {maxChargePower_ = val; needsUpdate().trigger();}

         double maxDischargePower() {return maxDischargePower_;}
         void setMaxDischargePower(double val) {maxDischargePower_ = val; needsUpdate().trigger();}

         double chargeEfficiency() {return chargeEfficiency_;}
         void setChargeEfficiency(double val) {chargeEfficiency_ = val; needsUpdate().trigger();}

         double dischargeEfficiency() {return dischargeEfficiency_;}
         void setDischargeEfficiency(double val) {dischargeEfficiency_ = val; needsUpdate().trigger();}

         double charge() {return charge_;}

         double requestedPower() {return requestedPower_;}
         void setRequestedPower(double val) {requestedPower_ = val; needsUpdate().trigger();}

         double internalPower() 
         {
            return requestedPower_ < 0 
            ? std::max(requestedPower_, -maxDischargePower_) / 
              dischargeEfficiency_
            : std::min(requestedPower_, maxChargePower_) * chargeEfficiency_;
         }

      // Member variables.
      private:
         // Parameters.
         Time dt_;                           // Timestep.
         double initCharge_;
         double maxCharge_;
         double maxChargePower_;
         double maxDischargePower_;
         double chargeEfficiency_;
         double dischargeEfficiency_;
                  
         // Setpoint.
         double requestedPower_;

         // State.
         double charge_;
   };
}
#endif // SIMPLE_BATTERY_DOT_H
