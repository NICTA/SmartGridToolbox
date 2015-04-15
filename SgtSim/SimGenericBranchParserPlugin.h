#ifndef SIM_GENERIC_BRANCH_PARSER_PLUGIN_DOT_H
#define SIM_GENERIC_BRANCH_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimNetworkComponent.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @brief ParserPlugin that parses generic branch objects.
    class SimGenericBranchParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "generic_branch";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // SIM_GENERIC_BRANCH_PARSER_PLUGIN_DOT_H
