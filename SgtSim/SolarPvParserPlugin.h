#ifndef SOLAR_PV_PARSER_DOT_H
#define SOLAR_PV_PARSER_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class SolarPvParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "solar_pv";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // SOLAR_PV_PARSER_DOT_H
