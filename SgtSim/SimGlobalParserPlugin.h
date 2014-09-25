#ifndef SIM_GLOBAL_PARSER_PLUGIN_DOT_H
#define SIM_GLOBAL_PARSER_PLUGIN_DOT_H

#include <SgtSim/Simulation.h>
#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses generic branch objects.
   class SimGlobalParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "simulation";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // SIM_GLOBAL_PARSER_PLUGIN_DOT_H
