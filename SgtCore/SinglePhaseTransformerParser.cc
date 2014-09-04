#include "SinglePhaseTransformerParser.h"

#include "SimBus.h"
#include "SimNetwork.h"
#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   void SinglePhaseTransformerParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SinglePhaseTransformer : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phase_0");
      assertFieldPresent(nd, "phase_1");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      string id = state.expandName(nd["id"].as<std::string>());
      Phase phase0 = nd["phase_0"].as<Phase>();
      Phase phase1 = nd["phase_1"].as<Phase>();
      Complex alpha = nd["complex_turns_ratio_01"].as<Complex>();
      Complex ZLeak = nd["leakage_impedance"].as<Complex>();

      mod.newComponent<SinglePhaseTransformer>(id, phase0, phase1, alpha, ZLeak);
   }

   void SinglePhaseTransformerParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SinglePhaseTransformer : postParse." << std::endl);

      string id = state.expandName(nd["id"].as<std::string>());
      SinglePhaseTransformer* comp = mod.component<SinglePhaseTransformer>(id);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      SimNetwork* networkComp = mod.component<SimNetwork>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << id <<  ", network " << networkStr
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
         error() << "For component " << id <<  ", bus " << bus0Str
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
         error() << "For component " << id <<  ", bus " << bus1Str
                 <<  " was not found in the model." << std::endl;
         abort();
      }
   }
}
