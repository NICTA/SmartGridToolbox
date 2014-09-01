#ifndef WEATHER_PARSER_DOT_H
#define WEATHER_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class WeatherParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "weather";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // WEATHER_PARSER_DOT_H
