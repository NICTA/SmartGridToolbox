#ifndef MATPOWER_PARSER
#define MATPOWER_PARSER

#include "Parser.h"

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

#endif // MATPOWER_PARSER
