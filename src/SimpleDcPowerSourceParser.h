#ifndef SIMPLE_DC_POWER_SOURCE_PARSER_DOT_H
#define SIMPLE_DC_POWER_SOURCE_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class SimpleDcPowerSourceParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "dc_power_source";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // SIMPLE_DC_POWER_SOURCE_PARSER_DOT_H
