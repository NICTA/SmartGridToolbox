#ifndef BUS_DOT_H
#define BUS_DOT_H

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
         Bus(const std::string & name) : Component(name) {}

         BusType type() const {return type_;}
         void setType(const BusType type) {type_ = type;}

         const Phases & phases() const {return phases_;}
         Phases & phases() {return phases_;}

         const UblasVector<Complex> & nominalV() const {return nominalV_;}
         UblasVector<Complex> & nominalV() {return nominalV_;}

         const UblasVector<Complex> & V() const {return V_;}
         UblasVector<Complex> & V() {return V_;}

         void addZipToGround(ZipToGroundBase & zipToGround);

         const UblasVector<Complex> & Y() const {return Y_;} // Injection.
         UblasVector<Complex> & Y() {return Y_;} // Injection.

         const UblasVector<Complex> & I() const {return I_;} // Injection.
         UblasVector<Complex> & I() {return I_;} // Injection.

         const UblasVector<Complex> & S() const {return S_;} // Injection.
         UblasVector<Complex> & S() {return S_;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         BusType type_;                                     ///< Bus type. 
         Phases phases_;                                    ///< Phases.
         std::vector<const ZipToGroundBase *> zipsToGround_;    ///< ZIP loads of generation.
         UblasVector<Complex> nominalV_;                    ///< Nominal voltage.
         UblasVector<Complex> V_;                           ///< Voltage.
         UblasVector<Complex> Y_;                           ///< Constant admittance shunt.
         UblasVector<Complex> I_;                           ///< Constant current injection.
         UblasVector<Complex> S_;                           ///< Constant power injection.
      /// @}
   };
}

#endif // BUS_DOT_H
