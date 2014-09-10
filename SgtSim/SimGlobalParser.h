#ifndef SIM_GLOBAL_PARSER_DOT_H
#define SIM_GLOBAL_PARSER_DOT_H

#include <SgtSim/Simulation.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses generic branch objects.
   class SimGlobalParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "simulation";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim) const override;
   };
}

#endif // SIM_GLOBAL_PARSER_DOT_H
