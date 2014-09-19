#ifndef SIMPLE_BUILDING_PARSER_DOT_H
#define SIMPLE_BUILDING_PARSER_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class SimpleBuilding;
   class Simulation;

   class SimpleBuildingParser : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "simple_building";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // SIMPLE_BUILDING_PARSER_DOT_H
