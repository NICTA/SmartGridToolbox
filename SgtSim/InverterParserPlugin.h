#ifndef INVERTER_ZIP_PARSER_PLUGIN_DOT_H
#define INVERTER_ZIP_PARSER_PLUGIN_DOT_H

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
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // INVERTER_ZIP_PARSER_PLUGIN_DOT_H
