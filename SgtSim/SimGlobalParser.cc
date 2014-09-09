#include "SimGlobalParser.h"

namespace SmartGridToolbox
{
   static Time parseTime(const YAML::Node & nd, local_time::time_zone_ptr timezone)
   {
      return timeFromLocalTime(nd.as<posix_time::ptime>(), timezone);
   }

   void SimGlobalParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      assertFieldPresent(nd, "start_time");
      assertFieldPresent(nd, "end_time");

      if (const YAML::Node & nodeTz = nd["timezone"])
      {
         try 
         {
            into.setTimezone(local_time::time_zone_ptr(new local_time::posix_time_zone(nodeTz.as<std::string>())));
         }
         catch (...)
         {
            error() << "Couldn't parse timezone " << nodeTz.as<std::string>() << "." << std::endl;
            abort();
         }
      }

      const YAML::Node & nodeStart = nd["start_time"];
      try 
      {
         into.setStartTime(parseTime(nodeStart, into.timezone()));
      }
      catch (...)
      {
         error() << "Couldn't parse start date " << nodeStart.as<std::string>() << "." << std::endl;
         abort();
      }

      const YAML::Node & nodeEnd = nd["end_time"];
      try 
      {
         into.setStartTime(parseTime(nodeStart, into.timezone()));
      }
      catch (...)
      {
         error() << "Couldn't parse end date " << nodeEnd.as<std::string>() << "." << std::endl;
         abort();
      }

      if (const YAML::Node & nodeLatLong = nd["lat_long"])
      {
         try 
         {
            std::vector<double> llvec = nodeLatLong.as<std::vector<double>>();
            if (llvec.size() != 2)
            {
               throw;
            };
            into.setLatLong({llvec[0], llvec[1]});
         }
         catch (...)
         {
            error() << "Couldn't parse lat_long " << nodeLatLong.as<std::string>() << "." << std::endl;
            abort();
         }
      }
   }
}
