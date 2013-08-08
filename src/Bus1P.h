#ifndef BUS_1P_DOT_H
#define BUS_1P_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"
#include "ZipToGround1P.h"

#include <iostream>

namespace SmartGridToolbox
{
   class ZipToGround1P;

   class Bus1PParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "bus_1_phase";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class Bus1P : public Component
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
         Bus1P(const std::string & name) : Component(name) {}

         BusType getType() const {return type_;}
         void setType(BusType type) {type_ = type;}

         const Complex & getV() const {return V_;}
         void setV(const Complex & V) {V_ = V;}

         void addZipToGround(ZipToGround1P & zipToGround);

         const Complex & getY() const {return Y_;} // Injection.
         const Complex & getI() const {return I_;} // Injection.
         const Complex & getS() const {return S_;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         std::vector<const ZipToGround1P *> zipsToGround_;
         BusType type_;          ///< Bus type. 
         Complex V_;             ///< Voltage.
         Complex Y_;             ///< Constant admittance shunt.
         Complex I_;             ///< Constant current injection.
         Complex S_;             ///< Constant power injection.
      /// @}
   };
}

#endif // BUS_1P_DOT_H
