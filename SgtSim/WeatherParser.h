#ifndef WEATHER_PARSER_DOT_H
#define WEATHER_PARSER_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class WeatherParser : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "weather";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // WEATHER_PARSER_DOT_H
