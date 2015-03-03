#ifndef HEARTBEAT_PARSER_PLUGIN_DOT_H
#define HEARTBEAT_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
   class Simulation;

   class HeartbeatParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "heartbeat";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // HEARTBEAT_PARSER_PLUGIN_DOT_H
