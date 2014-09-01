#ifndef SIMPLE_BUILDING_PARSER_DOT_H
#define SIMPLE_BUILDING_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class SimpleBuilding;
   class Simulation;

   class SimpleBuildingParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "simple_building";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SIMPLE_BUILDING_PARSER_DOT_H
