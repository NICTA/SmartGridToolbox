#ifndef CDF_PARSER_DOT_H
#define CDF_PARSER_DOT_H

#include "Parser.h"

namespace SmartGridToolbox
{
   class CDFParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "cdf";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // CDF_PARSER_DOT_H
