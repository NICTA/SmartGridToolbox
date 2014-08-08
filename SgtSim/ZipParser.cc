#include "ZipParser.h"

#include <SgtSim/Bus.h>
#include <SgtSim/Zip.h>

namespace SmartGridToolbox
{
   void ZipParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Zip : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases = nd["phases"].as<Phases>();

      Zip& comp = mod.newComponent<Zip>(name, phases);

      auto ndAdmit = nd["admittance"];
      auto ndCurLoad = nd["complex_current_load"];
      auto ndCurGen = nd["complex_current_gen"];
      auto ndSLoad = nd["complex_power_load"];
      auto ndSg = nd["complex_power_gen"];

      if (ndCurLoad and ndCurGen)
      {
         {
            error() << "A " << pluginKey() << " can't have both a current load and injection." << std::endl;
            abort();
         }
      }

      if (ndSLoad and ndSg)
      {
         {
            error() << "A " << pluginKey() << " can't have both a power load and injection." << std::endl;
            abort();
         }
      }

      if (ndAdmit)
      {
         comp.Y() = ndAdmit.as<ublas::vector<Complex>>();
      }
      if (ndCurLoad)
      {
         comp.I() = -ndCurLoad.as<ublas::vector<Complex>>();
      }
      if (ndCurGen)
      {
         comp.I() = ndCurGen.as<ublas::vector<Complex>>();
      }
      if (ndSLoad)
      {
         comp.S() = -ndSLoad.as<ublas::vector<Complex>>();
      }
      if (ndSg)
      {
         comp.S() = ndSg.as<ublas::vector<Complex>>();
      }
   }

   void ZipParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Zip : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Zip* zip = mod.component<Zip>(name);

      std::string busStr = state.expandName(nd["bus"].as<std::string>());
      Bus* bus = mod.component<Bus>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << name << ", bus " << busStr
                 << " was not found in the model." << std::endl;
         abort();
      }
      bus->addZip(*zip);
   }
}
