#include "Bus.h"
#include "Model.h"
#include "Network.h"
 
namespace SmartGridToolbox
{
   class ZipToGround;

   void BusParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Bus : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      const std::string nameStr = nd["name"].as<std::string>();
      Bus & comp = mod.newComponent<Bus>(nameStr);

      const std::string typeStr = nd["type"].as<std::string>();
      if (typeStr == "SL") 
      {
         comp.setType(BusType::SL);
      }
      else if (typeStr == "PQ") 
      {
         comp.setType(BusType::PQ);
      }
      else if (typeStr == "PV") 
      {
         comp.setType(BusType::PV);
      }
      else
      { 
         error() << "Bus type " << typeStr << " is not supported." << std::endl;
         abort();
      }

      if (nd["slack_voltage"])
      {
         comp.setV(nd["slack_voltage"].as<UblasVector<Complex>>());
      }
   }

   void BusParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Bus : postParse." << std::endl);
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus * comp = mod.getComponentNamed<Bus>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network * networkComp = mod.getComponentNamed<Network>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBus(*comp);
      }
      else
      {
         error() << "For component " << compNameStr << ", network " << networkStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }

   void Bus::updateState(ptime t0, ptime t1)
   {
      for (int i = 0; i < phases_.size(); ++i)
      {
         Y_(i) = I_(i) = S_(i) = {0.0, 0.0};
      }
      for (const ZipToGround * zip : zipsToGround_)
      {
         Y_ += zip->getY();
         I_ += zip->getI(); // Injection.
         S_ += zip->getS(); // Injection.
      }
   }

   void Bus::addZipToGround(ZipToGround & zipToGround)
   {
      zipsToGround_.push_back(&zipToGround);
      zipToGround.getEventDidUpdate().addAction([this](){getEventNeedsUpdate().trigger();}, 
            "Trigger Bus " + getName() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }
}
