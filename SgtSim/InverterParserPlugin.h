#ifndef INVERTER_PARSER_DOT_H
#define INVERTER_PARSER_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class InverterParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // INVERTER_PARSER_DOT_H
