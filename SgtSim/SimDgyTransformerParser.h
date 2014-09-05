#ifndef SIM_DGY_TRANSFORMER_PARSER
#define SIM_DGY_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses DgyTransformer objects.
   class SimDgyTransformerParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "sim_delta_g_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SIM_DGY_TRANSFORMER_PARSER
