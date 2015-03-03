#ifndef POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H
#define POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class Network;

   /// @brief ParserPlugin that parses PowerFlowSolver objects.
   class PowerFlowSolverParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "power_flow_solver";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
   };
}

#endif // POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H
