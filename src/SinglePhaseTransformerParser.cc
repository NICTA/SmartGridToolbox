#include "SinglePhaseTransformerParser.h"

#include "Network.h"
#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   void SinglePhaseTransformerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SinglePhaseTransformer : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phase_0");
      assertFieldPresent(nd, "phase_1");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      string name = state.expandName(nd["name"].as<std::string>());
      Phase phase0 = nd["phase_0"].as<Phase>();
      Phase phase1 = nd["phase_1"].as<Phase>();
      Complex alpha = nd["complex_turns_ratio_01"].as<Complex>();
      Complex ZLeak = nd["leakage_impedance"].as<Complex>();
      
      mod.newComponent<SinglePhaseTransformer>(name, phase0, phase1, alpha, ZLeak);
   }

   void SinglePhaseTransformerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SinglePhaseTransformer : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SinglePhaseTransformer * comp = mod.component<SinglePhaseTransformer>(name);

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
