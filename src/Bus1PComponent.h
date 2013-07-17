#ifndef BUS_1P_COMPONENT_DOT_H
#define BUS_1P_COMPONENT_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"

#include <iostream>

class NetworkComponent;

namespace SmartGridToolbox
{
   class Bus1PComponentParser : public ComponentParser
   {
      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, 
                                Model & mod) const override;

         static constexpr const char * getComponentName() 
         {
            return "bus_1_phase";
         }
   };

   class Bus1PComponent : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return time_pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         const Network1PComponent & getNetwork() {return *network_;}
         void setNetwork(Network1PComponent & network) {network_ = &network;}

         BusType getType() {return type_;}
         void setType(BusType type) {type_ = type;}

         const Complex & getV() {return V_;}
         void setV(const Complex & V) V_ = V;}

         const Complex & getY() {return Y_;}
         void setY(const Complex & Y) Y_ = Y;}

         const Complex & getI() {return I_;}
         void setI(const Complex & I) I_ = I;}

         const Complex & getS() {return S_;}
         void setS(const Complex & S) S_ = S;}

         const Complex & getSGen() {return SGen_;}
         void setSGen(const Complex & S) SGen_ = SGen;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Network1PComponent * network_;   ///< Network.
         BusType type_;                   ///< Bus type. 
         Complex V_;                      ///< Voltage.
         Complex Y_;                      ///< Constant admittance load.
         Complex I_;                      ///< Constant current load.
         Complex S_;                      ///< Constant power load.
         Complex SGen_;                   ///< Generator power.
      /// @}
   };
}

extern SmartGridToolbox::Bus1PComponentParser parser_;

#endif // BUS_1P_COMPONENT_DOT_H
