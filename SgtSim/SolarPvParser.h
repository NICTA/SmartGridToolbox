#ifndef SOLAR_PV_PARSER_DOT_H
#define SOLAR_PV_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class SolarPvParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "solar_pv";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // SOLAR_PV_PARSER_DOT_H
