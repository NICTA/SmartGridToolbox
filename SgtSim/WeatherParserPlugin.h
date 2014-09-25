#ifndef WEATHER_PARSER_PLUGIN_DOT_H
#define WEATHER_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class WeatherParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "weather";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // WEATHER_PARSER_PLUGIN_DOT_H
