#ifndef TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H
#define TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   class TimeSeriesZipParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "time_series_zip";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
   };
}

#endif // TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H
