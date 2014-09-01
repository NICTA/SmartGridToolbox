#ifndef SOLAR_PV_PARSER_DOT_H
#define SOLAR_PV_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class SolarPvParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "solar_pv";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SOLAR_PV_PARSER_DOT_H
