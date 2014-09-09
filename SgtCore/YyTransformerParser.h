#ifndef YY_TRANSFORMER_PARSER
#define YY_TRANSFORMER_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class YyTransformer;
   class Network;
   
   /// @brief ParserPlugin that parses YyTransformer objects.
   class YyTransformerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "yy_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<YyTransformer> parseYyTransformer(const YAML::Node& nd) const;
   };
}

#endif // YY_TRANSFORMER_PARSER
