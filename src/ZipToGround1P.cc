#include "ZipToGround1P.h"
#include "Bus1P.h"
#include "Model.h"

namespace SmartGridToolbox
{
   void ZipToGround1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "ZipToGround1P : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");

      const std::string nameStr = nd["name"].as<std::string>();
      ZipToGround1P & comp = mod.newComponent<ZipToGround1P>(nameStr);

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

      if (ndImp)
      {
         comp.setY(ndImp.as<Complex>());
      }
      if (ndCurLoad)
      {
         comp.setI(-ndCurLoad.as<Complex>());
      }
      if (ndCurGen)
      {
         comp.setI(ndCurGen.as<Complex>());
      }
      if (ndSLoad)
      {
         comp.setS(-ndSLoad.as<Complex>());
      }
      if (ndSGen)
      {
         comp.setS(ndSGen.as<Complex>());
      }
   }

   void ZipToGround1PParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "ZipToGround1P : postParse." << std::endl);
      const std::string compNameStr = nd["name"].as<std::string>();
      ZipToGround1P * zip = mod.getComponentNamed<ZipToGround1P>(compNameStr);
      std::string busStr = nd["bus"].as<std::string>();
      Bus1P * bus = mod.getComponentNamed<Bus1P>(busStr);
      if (bus == nullptr)
      {
         error() << "For component " << compNameStr << ", bus " << busStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
      bus->addZipToGround(*zip);
   }
}
