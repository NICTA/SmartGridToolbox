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
      BusType type = nd["type"].as<BusType>();
      Phases phases = nd["phases"].as<Phases>();
      int nPhase = phases.size();

      auto ndNominal = nd["nominal_voltage"];
      UblasVector<Complex> nominalV = ndNominal ? ndNominal.as<UblasVector<Complex>>()
                                                : UblasVector<Complex>(nPhase, czero);
      
      auto ndV = nd["voltage"];
      UblasVector<Complex> V = ndV ? ndV.as<UblasVector<Complex>>() : nominalV;

      Bus & comp = mod.newComponent<Bus>(name, type, phases, nominalV, V);
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
      Ys_ = UblasVector<Complex>(phases_.size(), czero);
      Ic_ = UblasVector<Complex>(phases_.size(), czero);
      Sc_ = UblasVector<Complex>(phases_.size(), czero);
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         Ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
      S_ = Sc_;
   }

   void Bus::updateState(Time t0, Time t1)
   {
      for (int i = 0; i < phases_.size(); ++i)
      {
         Ys_(i) = Ic_(i) = Sc_(i) = {0.0, 0.0};
      }
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         Ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
   }

   Bus::Bus(const std::string & name, BusType type, const Phases & phases, const UblasVector<Complex> & nominalV,
            const UblasVector<Complex> & V) :
      Component(name),
      type_(type),
      phases_(phases),
      nominalV_(nominalV),
      V_(V),
      Ys_(UblasVector<Complex>(phases.size(), czero)),
      Ic_(UblasVector<Complex>(phases.size(), czero)),
      Sc_(UblasVector<Complex>(phases.size(), czero))
   {
      assert(nominalV_.size() == phases_.size());
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
