#ifndef OVERHEAD_LINE_PARSER_DOT_H
#define OVERHEAD_LINE_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class OverheadLineParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "overhead_line";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // OVERHEAD_LINE_PARSER_DOT_H
