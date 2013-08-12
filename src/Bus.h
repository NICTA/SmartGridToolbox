#ifndef BUS_DOT_H
#define BUS_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"
#include "ZipToGround.h"

#include <iostream>

namespace SmartGridToolbox
{
   class ZipToGround;

   class BusParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "bus";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class Bus : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         Bus(const std::string & name) : Component(name) {}

         BusType getType() const {return type_;}
         void setType(BusType type) {type_ = type;}

         const std::vector<Phase> & getPhases() const {return phases_;}
         void setPhases(const std::vector<Phase> & phases) {phases_ = phases;}

         const UblasVector<Complex> & getV() const {return V_;}
         void setV(const UblasVector<Complex> & V) {V_ = V;}

         void addZipToGround(ZipToGround & zipToGround);

         const UblasVector<Complex> & getY() const {return Y_;} // Injection.
         const UblasVector<Complex> & getI() const {return I_;} // Injection.
         const UblasVector<Complex> & getS() const {return S_;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         BusType type_;                                     ///< Bus type. 
         std::vector<Phase> phases_;                        ///< Phases.
         std::vector<const ZipToGround *> zipsToGround_;    ///< ZIP loads of generation.
         UblasVector<Complex> V_;                           ///< Voltage.
         UblasVector<Complex> Y_;                           ///< Constant admittance shunt.
         UblasVector<Complex> I_;                           ///< Constant current injection.
         UblasVector<Complex> S_;                           ///< Constant power injection.
      /// @}
   };
}

#endif // BUS_DOT_H
