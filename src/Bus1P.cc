#include "Bus1P.h"
#include "Model.h"
#include "Network1P.h"
 
namespace SmartGridToolbox
{
   class ZipToGround1P;

   void Bus1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Bus1P : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      const std::string nameStr = nd["name"].as<std::string>();
      Bus1P & comp = mod.newComponent<Bus1P>(nameStr);

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
         comp.setV(nd["slack_voltage"].as<Complex>());
      }
   }

   void Bus1PParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Bus1P : postParse." << std::endl);
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus1P * comp = mod.getComponentNamed<Bus1P>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1P * networkComp = mod.getComponentNamed<Network1P>(networkStr);
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

   void Bus1P::updateState(ptime t0, ptime t1)
   {
      Y_ = I_ = S_ = {0.0, 0.0};
      for (const ZipToGround1P * zip : zipsToGround_)
      {
         Y_ += zip->getY();
         I_ += zip->getI(); // Injection.
         S_ += zip->getS(); // Injection.
      }
   }

   void Bus1P::addZipToGround(ZipToGround1P & zipToGround)
   {
      zipsToGround_.push_back(&zipToGround);
      zipToGround.getEventDidUpdate().addAction([this](){getEventNeedsUpdate().trigger();}, 
            "Trigger Bus1P " + getName() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }
}
