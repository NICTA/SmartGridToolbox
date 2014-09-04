#ifndef SIM_BUS_PARSER_DOT_H
#define SIM_BUS_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses SimBus objects.
   class SimBusParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "sim_bus";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SIM_BUS_PARSER_DOT_H
