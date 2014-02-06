#include "SimpleBuildingParser.h" 

#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/SimpleBuilding.h>

namespace SmartGridToolbox
{
   void SimpleBuildingParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleBuilding : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleBuilding & build = mod.newComponent<SimpleBuilding>(name);

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
   }

   void SimpleBuildingParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleBuilding : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleBuilding* build = mod.component<SimpleBuilding>(name);

      std::string busStr = state.expandName(nd["bus"].as<std::string>());
      Bus* bus = mod.component<Bus>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << name << ", bus " << busStr 
                 << " was not found in the model." << std::endl;
         abort();
      }
      bus->addZipToGround(*build);
   }
}
