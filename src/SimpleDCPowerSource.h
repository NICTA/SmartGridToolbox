#ifndef SIMPLE_DC_POWER_SOURCE
#define SIMPLE_DC_POWER_SOURCE

#include <smartgridtoolbox/DCPowerSourceBase.h>
#include <smartgridtoolbox/Parser.h>

namespace SmartGridToolbox
{
   class SimpleDCPowerSourceParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "dc_power_source";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   class SimpleDCPowerSource : public DCPowerSourceBase
   {
      /// @name Overridden member functions from DCPowerSourceBase.
      /// @{
      public:
         SimpleDCPowerSource(const std::string & name) : DCPowerSourceBase(name), PDC_(0.0) {}
         virtual double PDC() const {return PDC_;}
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
