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
      ublas::vector<Complex> nominalV = ndNominal ? ndNominal.as<ublas::vector<Complex>>()
                                                : ublas::vector<Complex>(nPhase, czero);
      
      auto ndV = nd["voltage"];
      ublas::vector<Complex> V = ndV ? ndV.as<ublas::vector<Complex>>() : nominalV;
      
      auto ndSGen = nd["complex_power_gen"];
      ublas::vector<Complex> SGen = ndSGen ? ndSGen.as<ublas::vector<Complex>>() : ublas::vector<Complex>(nPhase, czero);

      Bus & comp = mod.newComponent<Bus>(name, type, phases, nominalV, V, SGen);
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
      Ys_ = ublas::vector<Complex>(phases_.size(), czero);
      Ic_ = ublas::vector<Complex>(phases_.size(), czero);
      Sc_ = ublas::vector<Complex>(phases_.size(), czero);
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         Ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
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

   Bus::Bus(const std::string & name, BusType type, const Phases & phases, const ublas::vector<Complex> & nominalV,
            const ublas::vector<Complex> & V, const ublas::vector<Complex> & SGen) :
      Component(name),
      type_(type),
      phases_(phases),
      nominalV_(nominalV),
      V_(V),
      SGen_(SGen),
      Ys_(ublas::vector<Complex>(phases.size(), czero)),
      Ic_(ublas::vector<Complex>(phases.size(), czero)),
      Sc_(ublas::vector<Complex>(phases.size(), czero))
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
