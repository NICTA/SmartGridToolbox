#ifndef SIM_BUS_PARSER_DOT_H
#define SIM_BUS_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses SimBus objects.
   class SimBusParser : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "bus";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // SIM_BUS_PARSER_DOT_H
