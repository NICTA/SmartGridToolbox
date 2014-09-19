#ifndef SIM_DGY_TRANSFORMER_PARSER
#define SIM_DGY_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses DgyTransformer objects.
   class SimDgyTransformerParser : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "delta_g_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // SIM_DGY_TRANSFORMER_PARSER
