#include "SimGlobalParserPlugin.h"

namespace SmartGridToolbox
{
   namespace
   {
      Time parseTime(const YAML::Node& nd, local_time::time_zone_ptr timezone)
      {
         return timeFromLocalTime(nd.as<posix_time::ptime>(), timezone);
      }
   }

   void SimGlobalParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      assertFieldPresent(nd, "start_time");
      assertFieldPresent(nd, "end_time");

      if (const YAML::Node& nodeTz = nd["timezone"])
      {
         try 
         {
            sim.setTimezone(local_time::time_zone_ptr(new local_time::posix_time_zone(nodeTz.as<std::string>())));
         }
         catch (...)
         {
            Log().fatal() << "Couldn't parse timezone " << nodeTz.as<std::string>() << "." << std::endl;
         }
      }

      const YAML::Node& nodeStart = nd["start_time"];
      try 
      {
         sim.setStartTime(parseTime(nodeStart, sim.timezone()));
      }
      catch (...)
      {
         Log().fatal() << "Couldn't parse start date " << nodeStart.as<std::string>() << "." << std::endl;
      }

      const YAML::Node& nodeEnd = nd["end_time"];
      try 
      {
         sim.setEndTime(parseTime(nodeEnd, sim.timezone()));
      }
      catch (...)
      {
         Log().fatal() << "Couldn't parse end date " << nodeEnd.as<std::string>() << "." << std::endl;
      }

      if (const YAML::Node& nodeLatLong = nd["lat_long"])
      {
         try 
         {
            std::vector<double> llvec = nodeLatLong.as<std::vector<double>>();
            if (llvec.size() != 2)
            {
               throw;
            };
            sim.setLatLong({llvec[0], llvec[1]});
         }
         catch (...)
         {
            Log().fatal() << "Couldn't parse lat_long " << nodeLatLong.as<std::string>() << "." << std::endl;
         }
      }
   }

}
