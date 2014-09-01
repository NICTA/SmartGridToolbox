#ifndef INVERTER_PARSER_DOT_H
#define INVERTER_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class InverterParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // INVERTER_PARSER_DOT_H
