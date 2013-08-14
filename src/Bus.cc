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
      assertFieldPresent(nd, "phases");

      const std::string nameStr = nd["name"].as<std::string>();
      Bus & comp = mod.newComponent<Bus>(nameStr);
      comp.setPhases(nd["phases"].as<Phases>());
      comp.setType(nd["type"].as<BusType>());

      int nPhase = comp.getPhases().size();
      comp.setNominalV(UblasVector<Complex>(nPhase, czero));
      comp.setV(UblasVector<Complex>(nPhase, czero));
      auto ndNominal = nd["nominal_voltage"];
      if (ndNominal)
      {
         comp.setNominalV(ndNominal.as<UblasVector<Complex>>());
         comp.setV(comp.getNominalV());
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

   void Bus::initializeState(ptime t)
   {
      Y_ = UblasVector<Complex>(phases_.size(), czero);
      I_ = UblasVector<Complex>(phases_.size(), czero);
      S_ = UblasVector<Complex>(phases_.size(), czero);
      for (const ZipToGround * zip : zipsToGround_)
      {
         Y_ += zip->getY();
         I_ += zip->getI(); // Injection.
         S_ += zip->getS(); // Injection.
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
