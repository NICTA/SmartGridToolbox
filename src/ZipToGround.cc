#include "ZipToGround.h"
#include "Bus.h"
#include "Model.h"

namespace SmartGridToolbox
{
   void ZipToGroundParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "ZipToGround : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      auto ndName = nd["name"];
      auto ndPhases = nd["phases"];
      auto ndImp = nd["impedance"];
      auto ndCurLoad = nd["current_load"];
      auto ndCurGen = nd["current_gen"];
      auto ndSLoad = nd["complex_power_load"];
      auto ndSGen = nd["complex_power_gen"];

      if (ndCurLoad and ndCurGen)
      {
         {
            error() << "A " << getComponentName() << " can't have both a current load and injection." << std::endl;
            abort();
         }
      }

      if (ndSLoad and ndSGen)
      {
         {
            error() << "A " << getComponentName() << " can't have both a power load and injection." << std::endl;
            abort();
         }
      }

      const std::string nameStr = ndName.as<std::string>();
      ZipToGround & comp = mod.newComponent<ZipToGround>(nameStr);

      comp.setPhases(ndPhases.as<Phases>());
      int nPhase = comp.getPhases().size();

      // Defaults:
      comp.setY(UblasVector<Complex>(nPhase, czero));
      comp.setI(UblasVector<Complex>(nPhase, czero));
      comp.setS(UblasVector<Complex>(nPhase, czero));

      if (ndImp)
      {
         comp.setY(ndImp.as<UblasVector<Complex>>());
      }
      if (ndCurLoad)
      {
         comp.setI(-ndCurLoad.as<UblasVector<Complex>>());
      }
      if (ndCurGen)
      {
         comp.setI(ndCurGen.as<UblasVector<Complex>>());
      }
      if (ndSLoad)
      {
         comp.setS(-ndSLoad.as<UblasVector<Complex>>());
      }
      if (ndSGen)
      {
         comp.setS(ndSGen.as<UblasVector<Complex>>());
      }
   }

   void ZipToGroundParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "ZipToGround : postParse." << std::endl);
      const std::string compNameStr = nd["name"].as<std::string>();
      ZipToGround * zip = mod.getComponentNamed<ZipToGround>(compNameStr);
      std::string busStr = nd["bus"].as<std::string>();
      Bus * bus = mod.getComponentNamed<Bus>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << compNameStr << ", bus " << busStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
      bus->addZipToGround(*zip);
   }
}
