#ifndef SIM_SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN
#define SIM_SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN

#include <SgtSim/SimNetworkComponent.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @brief ParserPlugin that parses SinglePhaseTransformer objects.
    class SimSinglePhaseTransformerParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "single_phase_transformer";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // SIM_SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN
