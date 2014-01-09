#include <SmartGridToolbox/ZipToGround.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>

namespace SmartGridToolbox
{
   void ZipToGroundParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "ZipToGround : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases = nd["phases"].as<Phases>();

      ZipToGround & comp = mod.newComponent<ZipToGround>(name, phases);

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

   void ZipToGroundParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "ZipToGround : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      ZipToGround * zip = mod.component<ZipToGround>(name);

      std::string busStr = state.expandName(nd["bus"].as<std::string>());
      Bus * bus = mod.component<Bus>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << name << ", bus " << busStr 
                 << " was not found in the model." << std::endl;
         abort();
      }
      bus->addZipToGround(*zip);
   }

   ZipToGround::ZipToGround(const std::string & name, const Phases & phases) : 
      ZipToGroundBase(name, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
