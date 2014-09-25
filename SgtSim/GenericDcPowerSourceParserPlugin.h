#ifndef DC_POWER_SOURCE_PARSER_PLUGIN_DOT_H
#define DC_POWER_SOURCE_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class GenericDcPowerSourceParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "generic_dc_power_source";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // DC_POWER_SOURCE_PARSER_PLUGIN_DOT_H
