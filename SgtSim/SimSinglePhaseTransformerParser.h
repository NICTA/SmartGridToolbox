#ifndef SIM_SINGLE_PHASE_TRANSFORMER_PARSER
#define SIM_SINGLE_PHASE_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses SimSinglePhaseTransformer objects.
   class SimSinglePhaseTransformerParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "sim_single_phase_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
         
         std::unique_ptr<SimBranch> parseSimSimSinglePhaseTransformer(const YAML::Node& nd) const;
   };
}

#endif // SIM_SINGLE_PHASE_TRANSFORMER_PARSER
