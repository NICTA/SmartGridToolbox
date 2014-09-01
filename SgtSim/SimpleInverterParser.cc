#include "SimpleInverterParser.h"

#include "SimBus.h"
#include "SimpleInverter.h"

namespace SmartGridToolbox
{
   void SimpleInverterParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      string id = state.expandName(nd["id"].as<std::string>());
      Phases phases = nd["phases"].as<Phases>();

      SimpleInverter& comp = mod.newComponent<SimpleInverter>(id, phases);

      if (nd["efficiency"])
      {
         comp.setEfficiency(nd["efficiency"].as<double>());
      }

      if (nd["max_S_mag_per_phase"])
      {
         comp.setMaxSMagPerPhase(nd["max_S_mag_per_phase"].as<double>());
      }

      if (nd["min_power_factor"])
      {
         comp.setMinPowerFactor(nd["min_power_factor"].as<double>());
      }

      if (nd["requested_Q_per_phase"])
      {
         comp.setRequestedQPerPhase(nd["requested_Q_per_phase"].as<double>());
      }
   }

   void SimpleInverterParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : postParse." << std::endl);

      string id = state.expandName(nd["id"].as<std::string>());
      SimpleInverter& comp = *mod.component<SimpleInverter>(id);

      const std::string busStr = state.expandName(nd["bus"].as<std::string>());
      SimBus* busComp = mod.component<SimBus>(busStr);
      if (busComp != nullptr)
      {
         busComp->addZip(comp);
      }
      else
      {
         error() << "For component " << id << ", bus " << busStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }
}
