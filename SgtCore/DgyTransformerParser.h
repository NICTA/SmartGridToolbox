#ifndef DGY_TRANSFORMER_PARSER
#define DGY_TRANSFORMER_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class DgyTransformer;
   class Network;
   
   /// @brief ParserPlugin that parses DgyTransformer objects.
   class DgyTransformerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "delta_g_wye_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<DgyTransformer> parseDgyTransformer(const YAML::Node& nd) const;
   };
}

#endif // DGY_TRANSFORMER_PARSER
