#ifndef SIMPLE_INVERTER_PARSER_DOT_H
#define SIMPLE_INVERTER_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class SimpleInverterParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // SIMPLE_INVERTER_PARSER_DOT_H
