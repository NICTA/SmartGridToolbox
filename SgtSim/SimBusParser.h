#ifndef BUS_COMP_PARSER_DOT_H
#define BUS_COMP_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses SimBus objects.
   class SimBusParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key()
         {
            return "sim_bus";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& data) const;
         
         std::unique_ptr<SimBus> parseSimBus(const YAML::Node& nd) const;
   };
}

#endif // BUS_COMP_PARSER_DOT_H
