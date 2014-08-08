#ifndef WEATHER_PARSER_DOT_H
#define WEATHER_PARSER_DOT_H

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   class WeatherParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "weather";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // WEATHER_PARSER_DOT_H
