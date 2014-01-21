#include "YYTransformerParser.h"

#include "Network.h"
#include "YYTransformer.h"

namespace SmartGridToolbox
{
   void YYTransformerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "YYTransformer : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases0 = nd["phase_0"].as<Phases>();
      Phases phases1 = nd["phase_1"].as<Phases>();
      ublas::vector<Complex> alpha = nd["complex_turns_ratio_01"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> ZL = nd["leakage_impedance"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> ZM = nd["magnetising_impedance"].as<ublas::vector<Complex>>();
      
      mod.newComponent<YYTransformer>(name, phases0, phases1, alpha, ZL, ZM);
   }

   void YYTransformerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "YYTransformer : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      YYTransformer * comp = mod.component<YYTransformer>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network * networkComp = mod.component<Network>(networkStr);
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
      Bus * bus0Comp = mod.component<Bus>(bus0Str);
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
      Bus * bus1Comp = mod.component<Bus>(bus1Str);
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
