#ifndef SIMPLE_DC_POWER_SOURCE
#define SIMPLE_DC_POWER_SOURCE

#include "DCPowerSourceBase.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class SimpleDCPowerSourceParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "dc_power_source";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class SimpleDCPowerSource : public DCPowerSourceBase
   {
      /// @name Overridden member functions from DCPowerSourceBase.
      /// @{
      public:
         SimpleDCPowerSource(const std::string & name) : DCPowerSourceBase(name) {}
         virtual double getPDC() const {return PDC_;}
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         void setPDC(double PDC) {PDC_ = PDC;}
      /// @}
      
      /// @name My private member variables.
      /// @{
      public:
         double PDC_;
   };
}

#endif // DC_POWER_SOURCE
