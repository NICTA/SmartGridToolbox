#ifndef BUSc_DOT_H
#define BUSc_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"

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
         Bus(const std::string & name, BusType type, const Phases & phases, const UblasVector<Complex> & nominalV,
             const UblasVector<Complex> & V);

         BusType type() const {return type_;}

         const Phases & phases() const {return phases_;}

         const UblasVector<Complex> & nominalV() const {return nominalV_;}

         const UblasVector<Complex> & V() const {return V_;}
         UblasVector<Complex> & V() {return V_;}

         void addZipToGround(ZipToGroundBase & zipToGround);

         /// Total shunt admittance (loads).
         const UblasVector<Complex> & Ys() const {return Ys_;}
         UblasVector<Complex> & Ys() {return Ys_;}

         /// Total constant current injection (loads).
         const UblasVector<Complex> & Ic() const {return Ic_;}
         UblasVector<Complex> & Ic() {return Ic_;}

         /// Total constant power injection (loads).
         const UblasVector<Complex> & Sc() const {return Sc_;}
         UblasVector<Complex> & Sc() {return Sc_;}

         /// Total bus power injection (loads, gen).
         const UblasVector<Complex> & STot() const {return STot_;}
         UblasVector<Complex> & STot() {return STot_;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         BusType type_;                                        ///< Bus type.
         Phases phases_;                                       ///< Phases.

         std::vector<const ZipToGroundBase *> zipsToGround_;   ///< ZIP loads of generation.

         UblasVector<Complex> nominalV_;                       ///< Nominal voltage.

         UblasVector<Complex> V_;                              ///< Voltage.

         UblasVector<Complex> Ys_;                             ///< Constant admittance shunt (loads).
         UblasVector<Complex> Ic_;                             ///< Constant current injection (loads).
         UblasVector<Complex> Sc_;                             ///< Constant power injection (loads).

         UblasVector<Complex> STot_;                           ///< Total power injection.
      /// @}
   };
}

#endif // BUSc_DOT_H
