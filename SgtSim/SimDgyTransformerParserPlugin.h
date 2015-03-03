#ifndef SIM_DGY_TRANSFORMER_PARSER_PLUGIN
#define SIM_DGY_TRANSFORMER_PARSER_PLUGIN

#include <SgtSim/SimNetworkComponent.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
   class Simulation;

   /// @brief ParserPlugin that parses DgyTransformer objects.
   class SimDgyTransformerParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "delta_g_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // SIM_DGY_TRANSFORMER_PARSER_PLUGIN
