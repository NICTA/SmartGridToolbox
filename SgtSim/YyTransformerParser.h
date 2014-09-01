#ifndef YY_TRANSFORMER_PARSER
#define YY_TRANSFORMER_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class YyTransformerParser : public ParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "wye_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // YY_TRANSFORMER_PARSER
