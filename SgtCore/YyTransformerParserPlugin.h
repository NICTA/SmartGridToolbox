#ifndef YY_TRANSFORMER_PARSER_PLUGIN
#define YY_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class YyTransformer;
   class Network;
   
   /// @brief ParserPlugin that parses YyTransformer objects.
   class YyTransformerParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "yy_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
         
         std::unique_ptr<YyTransformer> parseYyTransformer(const YAML::Node& nd, const ParserBase& parser) const;
   };
}

#endif // YY_TRANSFORMER_PARSER_PLUGIN
