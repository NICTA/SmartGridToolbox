#ifndef MATPOWER_PARSER_DOT_H
#define MATPOWER_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class MatpowerParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "matpower";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // MATPOWER_PARSER_DOT_H
