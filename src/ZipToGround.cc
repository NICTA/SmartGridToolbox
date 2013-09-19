#include "ZipToGround.h"
#include "Bus.h"
#include "Model.h"

namespace SmartGridToolbox
{
   void ZipToGroundParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "ZipToGround : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      string name = state.expandName(nd["name"].as<std::string>());
      ZipToGround & comp = mod.newComponent<ZipToGround>(name);

      auto ndPhases = nd["phases"];
      auto ndAdmit = nd["admittance"];
      auto ndCurLoad = nd["current_load"];
      auto ndCurGen = nd["current_gen"];
      auto ndSLoad = nd["complex_power_load"];
      auto ndSGen = nd["complex_power_gen"];

      if (ndCurLoad and ndCurGen)
      {
         {
            error() << "A " << pluginKey() << " can't have both a current load and injection." << std::endl;
            abort();
         }
      }

      if (ndSLoad and ndSGen)
      {
         {
            error() << "A " << pluginKey() << " can't have both a power load and injection." << std::endl;
            abort();
         }
      }

      comp.phases() = ndPhases.as<Phases>();
      int nPhase = comp.phases().size();

      // Defaults:
      comp.Y() = UblasVector<Complex>(nPhase, czero);
      comp.I() = UblasVector<Complex>(nPhase, czero);
      comp.S() = UblasVector<Complex>(nPhase, czero);

      if (ndAdmit)
      {
         comp.Y() = ndAdmit.as<UblasVector<Complex>>();
      }
      if (ndCurLoad)
      {
         comp.I() = -ndCurLoad.as<UblasVector<Complex>>();
      }
      if (ndCurGen)
      {
         comp.I() = ndCurGen.as<UblasVector<Complex>>();
      }
      if (ndSLoad)
      {
         comp.S() = -ndSLoad.as<UblasVector<Complex>>();
      }
      if (ndSGen)
      {
         comp.S() = ndSGen.as<UblasVector<Complex>>();
      }
   }

   void ZipToGroundParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "ZipToGround : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      ZipToGround * zip = mod.componentNamed<ZipToGround>(name);

      std::string busStr = state.expandName(nd["bus"].as<std::string>());
      Bus * bus = mod.componentNamed<Bus>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << name << ", bus " << busStr 
                 << " was not found in the model." << std::endl;
         abort();
      }
      bus->addZipToGround(*zip);
   }
}
