#ifndef SIM_NETWORK_PARSER_DOT_H
#define SIM_NETWORK_PARSER_DOT_H

#include <SgtSim/SimNetwork.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses the network keyword, adding a new SimNetwork to the model.
   class SimNetworkParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key()
         {
            return "network";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& data) const;
         
         std::unique_ptr<SimNetwork> parseSimNetwork(const YAML::Node& nd) const;
   };
}

#endif // SIM_NETWORK_PARSER_DOT_H
