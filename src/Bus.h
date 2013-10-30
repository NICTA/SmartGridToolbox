#ifndef BUSc_DOT_H
#define BUSc_DOT_H

#include <smartgridtoolbox/Common.h>
#include <smartgridtoolbox/Component.h>
#include <smartgridtoolbox/Parser.h>
#include <smartgridtoolbox/PowerFlow.h>

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

         const ublas::vector<Complex> & V() const {return V_;}
         ublas::vector<Complex> & V() {return V_;}

         void addZipToGround(ZipToGroundBase & zipToGround);

         /// Total shunt admittance (loads).
         const ublas::vector<Complex> & Ys() const {return Ys_;}
         ublas::vector<Complex> & Ys() {return Ys_;}

         /// Total constant current injection (loads).
         const ublas::vector<Complex> & Ic() const {return Ic_;}
         ublas::vector<Complex> & Ic() {return Ic_;}

         /// Total constant power injection (loads).
         const ublas::vector<Complex> & Sc() const {return Sc_;}
         ublas::vector<Complex> & Sc() {return Sc_;}

         /// Generator power injection.
         const ublas::vector<Complex> & Sg() const {return Sg_;}
         ublas::vector<Complex> & Sg() {return Sg_;}
         
         /// Total power injection (loads + gen).
         const ublas::vector<Complex> STot() const {return Sc_ + Sg_;}
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
         ublas::vector<Complex> Ys_;                           ///< Constant admittance shunt (loads).
         ublas::vector<Complex> Ic_;                           ///< Constant current injection (loads).
         ublas::vector<Complex> Sc_;                           ///< Constant power injection (loads).
         /// @}
      /// @}
   };
}

#endif // BUSc_DOT_H
