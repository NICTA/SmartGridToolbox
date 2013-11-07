#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
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
         Bus(const std::string & name, BusType type, const Phases & phases, const ublas::vector<Complex> & nominalV,
             const ublas::vector<Complex> & V, const ublas::vector<Complex> & Sg);

         BusType type() const {return type_;}

         const Phases & phases() const {return phases_;}

         const ublas::vector<Complex> & nominalV() const {return nominalV_;}
         ublas::vector<Complex> & nominalV() {return nominalV_;}

         const ublas::vector<Complex> & V() const {return V_;}
         ublas::vector<Complex> & V() {return V_;}

         void addZipToGround(ZipToGroundBase & zipToGround);
         const std::vector<const ZipToGroundBase *> & zipsToGround() const {return zipsToGround_;}

         /// Total shunt admittance (sum of ZIPs).
         const ublas::vector<Complex> & ys() const {return ys_;}

         /// Total constant current injection (sum of ZIPs).
         const ublas::vector<Complex> & Ic() const {return Ic_;}

         /// Total constant power injection (sum of ZIPs).
         const ublas::vector<Complex> & Sc() const {return Sc_;}

         /// Generator power injection.
         const ublas::vector<Complex> & Sg() const {return Sg_;}
         ublas::vector<Complex> & Sg() {return Sg_;}
         
         /// Total power injection (Sc() + Sg()).
         const ublas::vector<Complex> STot() const {return Sc_ + Sg_;}
         
         /// Reactive power bounds.
         const ublas::vector<double> & QMin() const {return QMin_;}
         ublas::vector<double> & QMin() {return QMin_;}
         const ublas::vector<double> & QMax() const {return QMax_;}
         ublas::vector<double> & QMax() {return QMax_;}

         // TODO: bounds on other quantities e.g. theta etc.
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         BusType type_;                                        ///< Bus type.
         Phases phases_;                                       ///< Phases.

         std::vector<const ZipToGroundBase *> zipsToGround_;   ///< ZIP loads of generation.

         ublas::vector<Complex> nominalV_;                     ///< Nominal voltage.

         /// @name Quantities due to operation of bus:
         /** For PQ busses, the voltage is supposed to adjust so that the power injection matches Sc_. 
          *  Thus Sgen_ will be zero. For PV busses, the reactive power is supposed to adjust to keep the voltage
          *  magnitude constant. So Sgen_ will in general have zero real power and nonzero reactive power. 
          *  For slack busses, the both the real and reactive power will adjust to keep a constant voltage, 
          *  both components of Sg may be nonzero.*/
         /// @{
         ublas::vector<Complex> V_;                            ///< Voltage.
         ublas::vector<Complex> Sg_;                         ///< Generator power.
         /// @}

         /// @name ZIP load quantities:
         /// @{
         ublas::vector<Complex> ys_;                           ///< Constant admittance shunt (loads).
         ublas::vector<Complex> Ic_;                           ///< Constant current injection (loads).
         ublas::vector<Complex> Sc_;                           ///< Constant power injection (loads).

         ublas::vector<double> QMin_;                          ///< Reactive power min. 
         ublas::vector<double> QMax_;                          ///< Reactive power max. 
         /// @}
      /// @}
   };
}

#endif // BUS_DOT_H
