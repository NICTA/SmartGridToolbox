#ifndef YY_TRANSFORMER_PARSER
#define YY_TRANSFORMER_PARSER

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   class YyTransformerParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "wye_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // YY_TRANSFORMER_PARSER
