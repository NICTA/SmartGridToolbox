#ifndef BATTERY_PARSER_PLUGIN_DOT_H
#define BATTERY_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class BatteryParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "battery";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // BATTERY_PARSER_PLUGIN_DOT_H
