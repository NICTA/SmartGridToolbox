#ifndef DC_POWER_SOURCE_PARSER_DOT_H
#define DC_POWER_SOURCE_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class GenericDcPowerSourceParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "generic_dc_power_source";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // DC_POWER_SOURCE_PARSER_DOT_H
