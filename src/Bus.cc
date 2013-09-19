#include "Bus.h"
#include "Model.h"
#include "Network.h"
#include "ZipToGroundBase.h"
 
namespace SmartGridToolbox
{
   void BusParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Bus : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "nominal_voltage");

      string name = state.expandName(nd["name"].as<std::string>());
      Bus & comp = mod.newComponent<Bus>(name);

      comp.phases() = nd["phases"].as<Phases>();
      comp.setType(nd["type"].as<BusType>());

      int nPhase = comp.phases().size();
      comp.nominalV() = UblasVector<Complex>(nPhase, czero);
      comp.V() = UblasVector<Complex>(nPhase, czero);
      auto ndNominal = nd["nominal_voltage"];
      if (ndNominal)
      {
         comp.nominalV() = ndNominal.as<UblasVector<Complex>>();
         comp.V() = comp.nominalV();
      }
   }

   void BusParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Bus : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Bus * comp = mod.componentNamed<Bus>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network * networkComp = mod.componentNamed<Network>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBus(*comp);
      }
      else
      {
         error() << "For component " << name << ", network " << networkStr 
                 << " was not found in the model." << std::endl;
         abort();
      }
   }

   void Bus::initializeState()
   {
      Y_ = UblasVector<Complex>(phases_.size(), czero);
      I_ = UblasVector<Complex>(phases_.size(), czero);
      S_ = UblasVector<Complex>(phases_.size(), czero);
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         Y_ += zip->Y();
         I_ += zip->I(); // Injection.
         S_ += zip->S(); // Injection.
      }
   }

   void Bus::updateState(Time t0, Time t1)
   {
      for (int i = 0; i < phases_.size(); ++i)
      {
         Y_(i) = I_(i) = S_(i) = {0.0, 0.0};
      }
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         Y_ += zip->Y();
         I_ += zip->I(); // Injection.
         S_ += zip->S(); // Injection.
      }
   }

   void Bus::addZipToGround(ZipToGroundBase & zipToGround)
   {
      dependsOn(zipToGround);
      zipsToGround_.push_back(&zipToGround);
      zipToGround.didUpdate().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Bus " + name() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }
}
