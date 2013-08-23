#ifndef SIMPLE_INVERTER_DOT_H
#define SIMPLE_INVERTER_DOT_H

#include "InverterBase.h"

namespace SmartGridToolbox
{
   class SimpleInverterParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   /// SimpleInverter: DC power to n-phase AC converter.
   class SimpleInverter : public InverterBase
   {
      /// @name Overridden public member functions from InverterBase.
      /// @{
      public:
         virtual double getEfficiency(double powerDC) const override {return efficiency_;}
         virtual double getPhaseAngleRadians(double powerDC) const override {return phaseAngle_;}
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         SimpleInverter(const std::string & name) : InverterBase(name) {}

         void setEfficiency(double efficiency) {efficiency_ = efficiency;}

         void setPhaseAngleRadians(double phaseAngle) {phaseAngle_ = phaseAngle;}
      /// @}

      /// @name Private member variables.
      /// @{
      public:
         double efficiency_;
         double phaseAngle_;
      /// @}
   };
}

#endif // SIMPLE_INVERTER_DOT_H
