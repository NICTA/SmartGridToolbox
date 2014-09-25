#include "SimpleBuildingParserPlugin.h"

#include "SimpleBuilding.h"
#include "SimNetwork.h"
#include "Simulation.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void SimpleBuildingParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");

      std::string id = parser.expand<std::string>(nd["id"]);
      auto build = sim.newSimComponent<SimpleBuilding>(id);

      auto nd_dt = nd["dt"];
      if (nd_dt) build->set_dt(nd_dt.as<Time>());

      auto nd_kb = nd["kb"];
      if (nd_kb) build->set_kb(nd_kb.as<double>());

      auto ndCb = nd["Cb"];
      if (ndCb) build->setCb(ndCb.as<double>());

      auto ndTbInit = nd["Tb_init"];
      if (ndTbInit) build->setTbInit(ndTbInit.as<double>());

      auto nd_kh = nd["kh"];
      if (nd_kh) build->set_kh(nd_kh.as<double>());

      auto ndCopCool = nd["COP_cool"];
      if (ndCopCool) build->setCopCool(ndCopCool.as<double>());

      auto ndCopHeat = nd["COP_heat"];
      if (ndCopHeat) build->setCopHeat(ndCopHeat.as<double>());

      auto ndPMax = nd["P_max"];
      if (ndPMax) build->setPMax(ndPMax.as<double>());

      auto ndTs = nd["Ts"];
      if (ndTs) build->setTs(ndTs.as<double>());

      const auto& dQgNd = nd["internal_heat_power"];
      if (dQgNd)
      {
         std::string id = dQgNd.as<std::string>();
         auto series = sim.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            Log().fatal() << "Parsing simple_building: couldn't find time series " << id << std::endl;
         }
         build->set_dQgSeries(series);
      }

      std::string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string busId = parser.expand<std::string>(nd["bus_id"]);
      auto netw = sim.simComponent<SimNetwork>(netwId);
      netw->addZip(build, busId);

      const auto& weatherNd = nd["weather"];
      if (weatherNd)
      {
         std::string weatherStr = weatherNd.as<std::string>();
         auto weather = sim.simComponent<Weather>(weatherStr);
         if (weather == nullptr)
         {
            Log().fatal() << "Parsing simple_building: couldn't find weather " << weatherStr << std::endl;
         }
         build->setWeather(weather);
      }
   }
}
