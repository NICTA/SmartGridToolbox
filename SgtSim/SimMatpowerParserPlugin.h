#ifndef SIM_MATPOWER_PARSER_PLUGIN_DOT_H
#define SIM_MATPOWER_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimNetworkComponent.h>
#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses matpower networks.
   class SimMatpowerParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "matpower";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // SIM_MATPOWER_PARSER_PLUGIN_DOT_H
