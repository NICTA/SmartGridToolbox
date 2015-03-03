#ifndef TIME_SERIES_PARSER_PLUGIN_DOT_H
#define TIME_SERIES_PARSER_PLUGIN_DOT_H

#include <SgtSim/Simulation.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
   class Simulation;

   /// @brief ParserPlugin that parses generic branch objects.
   class TimeSeriesParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "time_series";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // TIME_SERIES_PARSER_PLUGIN_DOT_H
