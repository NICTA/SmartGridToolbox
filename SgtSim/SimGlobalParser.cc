#include "SimGlobalParser.h"

namespace SmartGridToolbox
{
   static Time parseTime(const YAML::Node& nd, local_time::time_zone_ptr timezone)
   {
      return timeFromLocalTime(nd.as<posix_time::ptime>(), timezone);
   }

   static void parseTimeSeries(const YAML::Node& nd, Simulation& sim)
   {
      Log().message() << "Parsing time series." << std::endl;
      Indent _;

      std::unique_ptr<TimeSeriesBase> ts;

      assertFieldPresent(nd, "id"); // data
      assertFieldPresent(nd, "type"); // data
      assertFieldPresent(nd, "value_type"); // real_scalar/complex_scalar/real_vector/complex_vector
      
      std::string id = nd["id"].as<std::string>();
      std::string type = nd["type"].as<std::string>();
      std::string valType = nd["value_type"].as<std::string>();

      Log().message() << "id = " << id << std::endl;

      bool isComplex = false;
      bool isVector = false;
      if (valType == "real_scalar")
      {
         isComplex = false;
         isVector = false;
      }
      else if (valType == "complex_scalar")
      {
         isComplex = true;
         isVector = false;
      }
      else if (valType == "real_vector")
      {
         isComplex = false;
         isVector = true;
      }
      else if (valType == "complex_vector")
      {
         isComplex = true;
         isVector = true;
      }
      else
      {
         Log().fatal() << "Bad data_type for time series." << std::endl;
      }

      if (type == "const_time_series")
      {
         assertFieldPresent(nd, "const_value");
         double v = nd["const_value"].as<double>();
         if (!isComplex && !isVector)
         {
            ts.reset(new ConstTimeSeries<Time, double>(v));
         }
         else
         {
            Log().fatal() << "Time series not yet supported." << std::endl;
         }
      }
      else if (type == "data_time_series")
      {
         assertFieldPresent(nd, "data_file");
         assertFieldPresent(nd, "interp_type");
         assertFieldPresent(nd, "relative_to_time");
         assertFieldPresent(nd, "time_unit");

         std::string dataFName = nd["data_file"].as<std::string>();
         std::ifstream infile(dataFName);
         if (!infile.is_open())
         {
            Log().fatal() << "Could not open the timeseries input file " << dataFName << "." << std::endl;
         }

         std::string interpType = nd["interp_type"].as<std::string>();

         std::string relto = nd["relative_to_time"].as<std::string>();

         posix_time::ptime pt = posix_time::time_from_string(relto);
         Time t0 = timeFromLocalTime(pt, sim.timezone());

         std::string time_unit = nd["time_unit"].as<std::string>();
         double to_secs = 1.0;
         if (time_unit == "s")
         {
            to_secs = 1.0;
         }
         else if (time_unit == "m")
         {
            to_secs = 60.0;
         }
         else if (time_unit == "h")
         {
            to_secs = 3600.0;
         }
         else if (time_unit == "d")
         {
            to_secs = 86400.0;
         }
         else
         {
            Log().fatal() << "Invalid time unit in data_time_series. Aborting." << std::endl;
         }

         if (!isComplex && !isVector)
         {
            std::unique_ptr<DataTimeSeries<Time, double>> dts;
            if (interpType == "stepwise")
            {
               dts.reset(new StepwiseTimeSeries<Time, double>());
            }
            else if (interpType == "lerp")
            {
               dts.reset(new LerpTimeSeries<Time, double>());
            }
            else if (interpType == "spline")
            {
               dts.reset(new SplineTimeSeries<Time>());
            }

            std::string line;
            while (std::getline(infile, line))
            {
               std::istringstream ss(line);
               std::string dateStr;
               std::string timeStr;
               double secs;
               double val;
               ss >> secs >> val;
               assert(ss.eof());
               secs *= to_secs;
               Time t = t0 + posix_time::seconds(secs);
               dts->addPoint(t, val); 
            }
            ts = std::move(dts);
         }
         else
         {
            Log().fatal() << "Time series not yet supported." << std::endl;
         }
      }
      else
      {
         Log().fatal() << "Bad time series type " << type << "." << std::endl;
      }
      sim.acquireTimeSeries(id, std::move(ts));
   }

   void SimGlobalParser::parse(const YAML::Node& nd, Simulation& sim) const
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

      if (const YAML::Node& ndTs = nd["time_series"])
      {
         parseTimeSeries(ndTs, sim); 
      }
   }

}
