#ifndef DGY_TRANSFORMER_PARSER
#define DGY_TRANSFORMER_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;
   
   /// @brief ParserPlugin that parses DgyTransformer objects.
   class DgyTransformerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "delta_g_wye_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // DGY_TRANSFORMER_PARSER
