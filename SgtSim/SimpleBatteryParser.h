#ifndef BATTERY_PARSER_DOT_H
#define BATTERY_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Battery;
   class Simulation;

   class BatteryParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key()
         {
            return "battery";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& data) const;
         
         std::unique_ptr<Battery> parseBattery(const YAML::Node& nd) const;
   };
}

#endif // BATTERY_PARSER_DOT_H
