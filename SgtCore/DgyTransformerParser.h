#ifndef DGY_TRANSFORMER_PARSER
#define DGY_TRANSFORMER_PARSER

#include <SgtCore/NetworkParser.h>

namespace SmartGridToolbox
{
   class DgyTransformer;
   class Network;
   
   /// @brief ParserPlugin that parses DgyTransformer objects.
   class DgyTransformerParser : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "delta_g_wye_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserState& state) const override;
         
         std::unique_ptr<DgyTransformer> parseDgyTransformer(const YAML::Node& nd, const ParserState& state) const;
   };
}

#endif // DGY_TRANSFORMER_PARSER
