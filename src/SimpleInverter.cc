#include "SimpleInverter.h"

#include "Model.h"
#include "Bus.h"

namespace SmartGridToolbox
{
   void SimpleInverterParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "phase_angle_degrees");

      const std::string nameStr = nd["name"].as<std::string>();
      SimpleInverter & comp = mod.newComponent<SimpleInverter>(nameStr);
      comp.phases() = nd["phases"].as<Phases>();
      comp.setPhaseAngleRadians(nd["phase_angle_degrees"].as<double>() * pi / 180.0);
   }

   void SimpleInverterParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : postParse." << std::endl);

      const std::string nameStr = nd["name"].as<std::string>();
      SimpleInverter & comp = *mod.componentNamed<SimpleInverter>(nameStr);

      const std::string busStr = nd["bus"].as<std::string>();
      Bus * busComp = mod.componentNamed<Bus>(busStr);
      if (busComp != nullptr)
      {
         busComp->addZipToGround(comp);
      }
      else
      {
         error() << "For component " << nameStr << ", bus " << busStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }
}
