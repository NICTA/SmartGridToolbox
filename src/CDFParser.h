#ifndef CDF_PARSER
#define CDF_PARSER

#include "Parser.h"

namespace SmartGridToolbox
{
   class CDFParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "CDF";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // CDF_PARSER
