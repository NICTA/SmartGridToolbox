#ifndef BATTERY_DOT_H
#define BATTERY_DOT_H

#include <SgtSim/DcPowerSourceBase.h>

#include <SgtCore/Common.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief Basic battery class, deriving from DCPowerSourceBase.
   class Battery : public DcPowerSourceBase
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         virtual Time validUntil() const override
         {
            return time() + dt_;
         }

      protected:
         virtual void initializeState() override
         {
            charge_ = initCharge_;
         }

         virtual void updateState(Time t) override;

      /// @}

      /// @name Overridden member functions from DcPowerSourceBase.
      /// @{
      
      public:
         virtual double PDc() const override; ///< Positive = charging.

      /// @}

      /// @name My public member functions.
      /// @{
      
      public:
         Battery(const std::string& id) :
            DcPowerSourceBase(id),
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

         Time dt() {return dt_;}
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

         double internalPower();

      /// @}

      private:
         // Parameters.
         Time dt_; ///< Timestep.
         double initCharge_;
         double maxCharge_;
         double maxChargePower_;
         double maxDischargePower_;
         double chargeEfficiency_;
         double dischargeEfficiency_;

         // Setpoint.
         double requestedPower_; ///< Positive = charging.

         // State.
         double charge_;
   };
}
#endif // BATTERY_DOT_H
