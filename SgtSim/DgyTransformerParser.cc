#include "DgyTransformerParser.h"

#include "SimBus.h"
#include "DgyTransformer.h"
#include "SimNetwork.h"

namespace SmartGridToolbox
{
   void DgyTransformerParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "DgyTransformer : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      string name = state.expandName(nd["name"].as<std::string>());
      Complex a = nd["complex_turns_ratio_01"].as<Complex>();
      Complex ZL = nd["leakage_impedance"].as<Complex>();

      mod.newComponent<DgyTransformer>(name, a, ZL);
   }

   void DgyTransformerParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "DgyTransformer : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      DgyTransformer* dgyTrans = mod.component<DgyTransformer>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      SimNetwork* networkComp = mod.component<SimNetwork>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(std::move(dgyTrans));
      }
      else
      {
         error() << "For component " << name <<  ", network " << networkStr
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      SimBus* bus0Comp = mod.component<SimBus>(bus0Str);
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
      SimBus* bus1Comp = mod.component<SimBus>(bus1Str);
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
