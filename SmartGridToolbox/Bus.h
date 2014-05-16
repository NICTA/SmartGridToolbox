#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/Event.h>
#include <SmartGridToolbox/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class ZipToGroundBase;

   /// @brief A Bus component of a Network.
   /// @ingroup PowerFlowCore
   class Bus : public Component
   {
      friend class BusParser;

      /// @name Overridden member functions from Component.
      /// @{

      public:
         // virtual Time validUntil() const override;

      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;
      
      /// @}
      
      public:
      /// @name Lifecycle.
      /// @{
         
         Bus(const std::string& name, BusType type, const Phases& phases, const ublas::vector<Complex>& nominalV);
      
      /// @}

      /// @name Basic info.
      /// @{
         
         BusType type() const {return type_;}
         const Phases& phases() const {return phases_;}
         const ublas::vector<Complex>& nominalV() const {return nominalV_;}
      
      /// @}

      /// @name Real generated power setpoints.
      /// For PV busses.
      /// @{
         
         ublas::vector<double> PgSetpoint() const
         {
            return PgSetpoint_;
         }

         void setPgSetpoint(const ublas::vector<double>& PgSetpoint);

      /// @}

      /// @name Voltage magnitude setpoints.
      /// For SL, PV busses.
      /// @{
         
         ublas::vector<double> VMagSetpoint() const
         {
            return VMagSetpoint_;
         }

         void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint);

      /// @}

      /// @name Voltage angle (radians) setpoints.
      /// For SL busses.
      /// @{
         
         ublas::vector<double> VAngSetpoint() const
         {
            return VAngSetpoint_;
         }

         void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint);

      /// @}

      /// @name Apply setpoints.
      /// @{
         
         /// @brief Set quantities such as voltage, power according to the setpoints.
         void applySetpoints();

      /// @}
      
      /// @name Loads/constant power generation.
      /// ZIP = constant Z, I, P (or Y, I, S).
      /// @{
         
         const std::vector<ZipToGroundBase*>& zipsToGround() const {return zipsToGround_;}

         void addZipToGround(ZipToGroundBase& zipToGround);

         /// @brief Total shunt admittance (sum of ZIPs).
         const ublas::vector<Complex>& Ys() const {return Ys_;}

         /// @brief Total constant current injection (sum of ZIPs).
         const ublas::vector<Complex>& Ic() const {return Ic_;}

         /// @brief Total constant power injection (sum of ZIPs).
         const ublas::vector<Complex>& Sc() const {return Sc_;}
      
      /// @}

      /// @name State.
      /// @{
         
         /// @brief Get bus voltage.
         const ublas::vector<Complex>& V() const {return V_;}

         /// @brief Set bus voltage (warm start or solver).
         void setV(const ublas::vector<Complex>& V) {V_ = V;}

         /// @brief Get bus generated power.
         const ublas::vector<Complex>& Sg() const {return Sg_;}

         /// @brief Set bus generated power (warm start or solver).
         void setSg(const ublas::vector<Complex>& Sg) {Sg_ = Sg;}

         /// @brief Total power injection (Sc() + Sg()).
         const ublas::vector<Complex> STot() const {return Sc_ + Sg_;}
      
      /// @}

      /// @name Events.
      /// @{
         
         /// @brief Event triggered when bus (e.g. setpoint) has changed.
         Event& changed() {return changed_;}
      
      /// @}

      private:
         BusType type_;                                ///< Bus type.
         Phases phases_;                               ///< Phases.
         ublas::vector<Complex> nominalV_;             ///< Nominal/base voltage.

         ublas::vector<double> PgSetpoint_;            ///< For PV bus.
         ublas::vector<double> VMagSetpoint_;          ///< For PV or SL bus.
         ublas::vector<double> VAngSetpoint_;          ///< For SL bus only.

         std::vector<ZipToGroundBase*> zipsToGround_;  ///< ZIP loads of generation.

      /// @name Bus Quantities
      /// @brief Quantities due to operation of bus.
      ///
      /// For PQ busses, the voltage is supposed to adjust so that the power injection matches Sc_.
      /// Thus Sgen_ will be zero. For PV busses, the reactive power is supposed to adjust to keep the voltage
      /// magnitude constant. So Sgen_ will in general have zero real power and nonzero reactive power.
      /// For slack busses, the both the real and reactive power will adjust to keep a constant voltage,
      /// both components of Sg may be nonzero.
      /// @{
         ublas::vector<Complex> V_;  ///< Voltage.
         ublas::vector<Complex> Sg_; ///< Generator power.
      /// @}

      /// @name cached ZIP load quantities:
      /// @{
         ublas::vector<Complex> Ys_; ///< Constant admittance shunt (loads).
         ublas::vector<Complex> Ic_; ///< Constant current injection (loads).
         ublas::vector<Complex> Sc_; ///< Constant power injection (loads).
      /// @}

      /// @name Events:
      /// @{
         Event changed_;
      /// @}
   };
}

#endif // BUS_DOT_H
