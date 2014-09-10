#ifndef SINGLE_PHASE_TRANSFORMER_PARSER
#define SINGLE_PHASE_TRANSFORMER_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class SinglePhaseTransformer;
   class Network;
   
   /// @brief ParserPlugin that parses SinglePhaseTransformer objects.
   class SinglePhaseTransformerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "single_phase_transformer";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
         
         std::unique_ptr<SinglePhaseTransformer> parseSinglePhaseTransformer(const YAML::Node& nd) const;
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_PARSER
