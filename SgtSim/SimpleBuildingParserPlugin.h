#ifndef SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H
#define SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class SimpleBuilding;
   class Simulation;

   class SimpleBuildingParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "simple_building";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H
