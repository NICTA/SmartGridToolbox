#ifndef BUS_PARSER_DOT_H
#define BUS_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class BusParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "bus";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // BUS_PARSER_DOT_H
