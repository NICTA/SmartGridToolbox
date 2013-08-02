#ifndef BUS_1P_DOT_H
#define BUS_1P_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"
#include "ZipLoad1P.h"

#include <iostream>

namespace SmartGridToolbox
{
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
         BusType getType() const {return type_;}
         void setType(BusType type) {type_ = type;}

         const Complex & getV() const {return V_;}
         void setV(const Complex & V) {V_ = V;}

         void addZipLoad(ZipLoad1P & zipLoad);

         const Complex & getY() const {return Y_;}
         void setY(const Complex & Y) {Y_ = Y;} // TODO: this will go since will be set by ziploads only.
         const Complex & getI() const {return I_;}
         void setI(const Complex & I) {I_ = I;} // TODO: this will go since will be set by ziploads only.
         const Complex & getS() const {return S_;}
         void setS(const Complex & S) {S_ = S;} // TODO: this will go since will be set by ziploads only.

         const Complex & getSGen() const {return SGen_;}
         void setSGen(const Complex & SGen) {SGen_ = SGen;}
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         std::vector<const ZipLoad1P *> zipLoads_;
         BusType type_;          ///< Bus type. 
         Complex V_;             ///< Voltage.
         Complex Y_;             ///< Constant admittance load.
         Complex I_;             ///< Constant current load.
         Complex S_;             ///< Constant power load.
         Complex SGen_;          ///< Generator power.
      /// @}
   };
}

#endif // BUS_1P_DOT_H
