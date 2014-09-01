#ifndef SIMPLE_INVERTER_PARSER_DOT_H
#define SIMPLE_INVERTER_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class SimpleInverterParser : public ParserPlugin
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

#endif // SIMPLE_INVERTER_PARSER_DOT_H
