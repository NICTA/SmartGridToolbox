#ifndef SIMPLE_BATTERY_PARSER_DOT_H
#define SIMPLE_BATTERY_PARSER_DOT_H

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   class SimpleBatteryParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "simple_battery";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // SIMPLE_BATTERY_PARSER_DOT_H
