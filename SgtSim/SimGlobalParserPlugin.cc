#include "SimGlobalParserPlugin.h"

namespace SmartGridToolbox
{
   namespace
   {
      Time parseTime(const YAML::Node& nd, local_time::time_zone_ptr timezone, const ParserBase& parser)
      {
         return timeFromLocalTime(parser.expand<posix_time::ptime>(nd), timezone);
      }
   }

   void SimGlobalParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "start_time");
      assertFieldPresent(nd, "end_time");

      if (const YAML::Node& nodeTz = nd["timezone"])
      {
         try 
         {
            sim.setTimezone(
                  local_time::time_zone_ptr(new local_time::posix_time_zone(parser.expand<std::string>(nodeTz))));
         }
         catch (...)
         {
            Log().fatal() << "Couldn't parse timezone " << parser.expand<std::string>(nodeTz) << "." << std::endl;
         }
      }

      const YAML::Node& nodeStart = nd["start_time"];
      try 
      {
         sim.setStartTime(parseTime(nodeStart, sim.timezone(), parser));
      }
      catch (...)
      {
         Log().fatal() << "Couldn't parse start date " << parser.expand<std::string>(nodeStart) << "." << std::endl;
      }

      const YAML::Node& nodeEnd = nd["end_time"];
      try 
      {
         sim.setEndTime(parseTime(nodeEnd, sim.timezone(), parser));
      }
      catch (...)
      {
         Log().fatal() << "Couldn't parse end date " << parser.expand<std::string>(nodeEnd) << "." << std::endl;
      }

      if (const YAML::Node& nodeLatLong = nd["lat_long"])
      {
         try 
         {
            std::vector<double> llvec = parser.expand<std::vector<double>>(nodeLatLong);
            if (llvec.size() != 2)
            {
               throw;
            };
            sim.setLatLong({llvec[0], llvec[1]});
         }
         catch (...)
         {
            Log().fatal() << "Couldn't parse lat_long " << parser.expand<std::string>(nodeLatLong) << "." << std::endl;
         }
      }
   }

}
