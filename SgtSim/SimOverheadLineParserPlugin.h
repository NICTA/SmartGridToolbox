#ifndef SIM_OVERHEAD_LINE_PARSER_DOT_H
#define SIM_OVERHEAD_LINE_PARSER_DOT_H

#include "SimNetworkComponent.h"

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses OverheadLine objects.
   class SimOverheadLineParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "overhead_line";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // OVERHEAD_LINE_PARSER_DOT_H
