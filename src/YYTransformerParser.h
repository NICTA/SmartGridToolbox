#ifndef Y_Y_TRANSFORMER_PARSER
#define Y_Y_TRANSFORMER_PARSER

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class YYTransformerParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "Y_Y_transformer";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // Y_Y_TRANSFORMER_PARSER
