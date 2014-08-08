#include "YyTransformerParser.h"

#include "BusComp.h"
#include "NetworkComp.h"
#include "YyTransformer.h"

namespace SmartGridToolbox
{
   void YyTransformerParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "YyTransformer : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();
      Complex a = nd["complex_turns_ratio_01"].as<Complex>();
      Complex ZL = nd["leakage_impedance"].as<Complex>();
      YAML::Node nd_mag_admit = nd["magnetising_admittance"];
      Complex YM = nd_mag_admit ? nd["magnetising_admittance"].as<Complex>() : czero;

      mod.newComponent<YyTransformer>(name, phases0, phases1, a, ZL, YM);
   }

   void YyTransformerParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "YyTransformer : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      YyTransformer* comp = mod.component<YyTransformer>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      NetworkComp* networkComp = mod.component<NetworkComp>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << name <<  ", network " << networkStr
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      BusComp* bus0Comp = mod.component<BusComp>(bus0Str);
      if (networkComp != nullptr)
      {
         comp->setBus0(*bus0Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus0Str
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus1Str = state.expandName(nd["bus_1"].as<std::string>());
      BusComp* bus1Comp = mod.component<BusComp>(bus1Str);
      if (networkComp != nullptr)
      {
         comp->setBus1(*bus1Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus1Str
                 <<  " was not found in the model." << std::endl;
         abort();
      }
   }
}
