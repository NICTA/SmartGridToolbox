#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/Event.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class ZipToGroundBase;

   class BusParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "bus";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   class Bus : public Component
   {
      friend class BusParser;

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState() override;
         virtual void updateState(Time t0, Time t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         /// @name Lifecycle.
         /// @{
         Bus(const std::string & name, BusType type, const Phases & phases, const ublas::vector<Complex> & nominalV);
         /// @}

         /// @name Basic info.
         /// @{
         BusType type() const {return type_;}
         const Phases & phases() const {return phases_;}
         const ublas::vector<Complex> & nominalV() const {return nominalV_;}
         /// @}

         /// @name Setpoints.
         /// @{
         /// @name Real generated power.
         /** Equality for PQ, PV, bounds for SL. */
         /// @{
         ublas::vector<double> PgSetpoint() const
         {
            return PgSetpoint_;
         }
         void setPgSetpoint(const ublas::vector<double> & PgSetpoint);
         ublas::vector<double> PgMinSetpoint() const
         {
            return PgMinSetpoint_;
         }
         void setPgMinSetpoint(const ublas::vector<double> & PgMinSetpoint);
         ublas::vector<double> PgMaxSetpoint() const
         {
            return PgMaxSetpoint_;
         }
         void setPgMaxSetpoint(const ublas::vector<double> & PgMaxSetpoint);
         /// @}

         /// @name Reactive generated power.
         /** Equality for PQ, bounds for SL, PV. */
         /// @{
         ublas::vector<double> QgSetpoint() const
         {
            return QgSetpoint_;
         }
         void setQgSetpoint(const ublas::vector<double> & QgSetpoint);
         ublas::vector<double> QgMinSetpoint() const
         {
            return QgMinSetpoint_;
         }
         void setQgMinSetpoint(const ublas::vector<double> & QgMinSetpoint);
         ublas::vector<double> QgMaxSetpoint() const
         {
            return QgMaxSetpoint_;
         }
         void setQgMaxSetpoint(const ublas::vector<double> & QgMaxSetpoint);
         /// @}

         /// @name Voltage magnitude.
         /** Equality for SL, PV, bounds for PQ. */
         /// @{
         ublas::vector<double> VMagSetpoint() const
         {
            return VMagSetpoint_;
         }
         void setVMagSetpoint(const ublas::vector<double> & VMagSetpoint);
         ublas::vector<double> VMagMinSetpoint() const
         {
            return VMagMinSetpoint_;
         }
         void setVMagMinSetpoint(const ublas::vector<double> & VMagMinSetpoint);
         ublas::vector<double> VMagMaxSetpoint() const
         {
            return VMagMaxSetpoint_;
         }
         void setVMagMaxSetpoint(const ublas::vector<double> & VMagMaxSetpoint);
         /// @}

         /// @name Voltage angle (radians).
         /** Equality for SL, bounds for PQ, PV. */
         /// @{
         ublas::vector<double> VAngSetpoint() const
         {
            return VAngSetpoint_;
         }
         void setVAngSetpoint(const ublas::vector<double> & VAngSetpoint);
         ublas::vector<double> VAngMinSetpoint() const
         {
            return VAngMinSetpoint_;
         }
         void setVAngMinSetpoint(const ublas::vector<double> & VAngMinSetpoint);
         ublas::vector<double> VAngMaxSetpoint() const
         {
            return VAngMaxSetpoint_;
         }
         void setVAngMaxSetpoint(const ublas::vector<double> & VAngMaxSetpoint);
         /// @}
         /// @}

         /// @name Loads / constant power generation.
         /// @{
         /// ZIP = constant Z, I, P (or Y, I, S).
         const std::vector<const ZipToGroundBase *> & zipsToGround() const {return zipsToGround_;}
         /// ZIP = constant Z, I, P (or Y, I, S).
         void addZipToGround(ZipToGroundBase & zipToGround);

         /// Total shunt admittance (sum of ZIPs).
         const ublas::vector<Complex> & ys() const {return ys_;}

         /// Total constant current injection (sum of ZIPs).
         const ublas::vector<Complex> & Ic() const {return Ic_;}

         /// Total constant power injection (sum of ZIPs).
         const ublas::vector<Complex> & Sc() const {return Sc_;}
         /// @}

         /// @name State.
         /// @{
         /// Get bus voltage.
         const ublas::vector<Complex> & V() const {return V_;}
         /// Set bus voltage (warm start or solver).
         void setV(const ublas::vector<Complex> & V) {V_ = V;}

         /// Get bus generated power.
         const ublas::vector<Complex> & Sg() const {return Sg_;}
         /// Set bus generated power (warm start or solver).
         void setSg(const ublas::vector<Complex> & Sg) {Sg_ = Sg;}

         /// Total power injection (Sc() + Sg()).
         const ublas::vector<Complex> STot() const {return Sc_ + Sg_;}
         /// @}

         /// @name Events.
         /// @{
         Event & setpointChanged() {return setpointChanged_;}
         /// @}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         BusType type_;                    ///< Bus type.
         Phases phases_;                   ///< Phases.
         ublas::vector<Complex> nominalV_; ///< Nominal voltage.

         ublas::vector<double> PgSetpoint_;      ///< For PQ or PV bus.
         ublas::vector<double> PgMinSetpoint_;   ///< For SL bus.
         ublas::vector<double> PgMaxSetpoint_;   ///< For SL bus.

         ublas::vector<double> QgSetpoint_;      ///< For PQ bus.
         ublas::vector<double> QgMinSetpoint_;   ///< For PV or SL bus.
         ublas::vector<double> QgMaxSetpoint_;   ///< For PV or SL bus.

         ublas::vector<double> VMagSetpoint_;    ///< For PV or SL bus.
         ublas::vector<double> VMagMinSetpoint_; ///< For PQ bus.
         ublas::vector<double> VMagMaxSetpoint_; ///< For PQ bus.

         ublas::vector<double> VAngSetpoint_;    ///< For SL bus.
         ublas::vector<double> VAngMinSetpoint_; ///< For PV bus.
         ublas::vector<double> VAngMaxSetpoint_; ///< For PV bus.

         std::vector<const ZipToGroundBase *> zipsToGround_; ///< ZIP loads of generation.

         /// @name Quantities due to operation of bus:
         /** For PQ busses, the voltage is supposed to adjust so that the power injection matches Sc_.
          *  Thus Sgen_ will be zero. For PV busses, the reactive power is supposed to adjust to keep the voltage
          *  magnitude constant. So Sgen_ will in general have zero real power and nonzero reactive power.
          *  For slack busses, the both the real and reactive power will adjust to keep a constant voltage,
          *  both components of Sg may be nonzero.*/
         /// @{
         ublas::vector<Complex> V_;  ///< Voltage.
         ublas::vector<Complex> Sg_; ///< Generator power.
         /// @}

         /// @name cached ZIP load quantities:
         /// @{
         ublas::vector<Complex> ys_; ///< Constant admittance shunt (loads).
         ublas::vector<Complex> Ic_; ///< Constant current injection (loads).
         ublas::vector<Complex> Sc_; ///< Constant power injection (loads).
         /// @}
         
         /// @name Events:
         /// @{
         Event setpointChanged_;
         /// @}
      /// @}
   };
}

#endif // BUS_DOT_H
