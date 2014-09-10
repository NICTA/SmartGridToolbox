#ifndef BATTERY_PARSER_DOT_H
#define BATTERY_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class BatteryParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "battery";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim) const override;
   };
}

#endif // BATTERY_PARSER_DOT_H
