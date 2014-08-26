#ifndef DGY_TRANSFORMER_PARSER
#define DGY_TRANSFORMER_PARSER

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses DgyTransformer objects.
   class DgyTransformerParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "delta_g_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // DGY_TRANSFORMER_PARSER
