#include "SimpleBuildingParser.h"

#include <SgtSim/BusComp.h>
#include <SgtSim/SimpleBuilding.h>

namespace SmartGridToolbox
{
   void SimpleBuildingParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimpleBuilding : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleBuilding& build = mod.newComponent<SimpleBuilding>(name);

      auto nd_dt = nd["dt"];
      if (nd_dt) build.set_dt(nd_dt.as<Time>());

      auto nd_kb = nd["kb"];
      if (nd_kb) build.set_kb(nd_kb.as<double>());

      auto ndCb = nd["Cb"];
      if (ndCb) build.setCb(ndCb.as<double>());

      auto ndTbInit = nd["Tb_init"];
      if (ndTbInit) build.setTbInit(ndTbInit.as<double>());

      auto nd_kh = nd["kh"];
      if (nd_kh) build.set_kh(nd_kh.as<double>());

      auto ndCopCool = nd["COP_cool"];
      if (ndCopCool) build.setCopCool(ndCopCool.as<double>());

      auto ndCopHeat = nd["COP_heat"];
      if (ndCopHeat) build.setCopHeat(ndCopHeat.as<double>());

      auto ndPMax = nd["P_max"];
      if (ndPMax) build.setPMax(ndPMax.as<double>());

      auto ndTs = nd["Ts"];
      if (ndTs) build.setTs(ndTs.as<double>());

      const auto& dQgNd = nd["internal_heat_power"];
      if (dQgNd)
      {
         std::string name = dQgNd.as<std::string>();
         const TimeSeries<Time, double>*series = mod.timeSeries<TimeSeries<Time, double>>(name);
         if (series == nullptr)
         {
            error() << "Parsing simple_building: couldn't find time series " << name << std::endl;
            abort();
         }
         build.set_dQgSeries(*series);
      }
   }

   void SimpleBuildingParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimpleBuilding : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleBuilding* build = mod.component<SimpleBuilding>(name);

      std::string busStr = state.expandName(nd["bus"].as<std::string>());
      BusComp* bus = mod.component<BusComp>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << name << ", bus " << busStr
                 << " was not found in the model." << std::endl;
         abort();
      }
      bus->addZip(*build);

      const auto& weatherNd = nd["weather"];
      if (weatherNd)
      {
         std::string weatherStr = weatherNd.as<std::string>();
         const Weather* weather = mod.component<Weather>(weatherStr);
         if (weather == nullptr)
         {
            error() << "Parsing simple_building: couldn't find weather " << weatherStr << std::endl;
            abort();
         }
         build->setWeather(*weather);
      }
   }
}
