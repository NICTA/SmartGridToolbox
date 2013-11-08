#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/ZipToGroundBase.h>
 
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
      
      Bus & bus = mod.newComponent<Bus>(name, type, phases, nominalV);

      auto ndPg = nd["P_gen_setpt"];
      if (ndPg) bus.setPgSetpt(ndPg.as<ublas::vector<double>>());

      auto ndPgMin = nd["P_gen_min_setpt"];
      if (ndPgMin) bus.setPgMinSetpt(ndPgMin.as<ublas::vector<double>>());
      
      auto ndPgMax = nd["P_gen_max_setpt"];
      if (ndPgMax) bus.setPgMaxSetpt(ndPgMax.as<ublas::vector<double>>());

      auto ndQg = nd["Q_gen_setpt"];
      if (ndQg) bus.setQgSetpt(ndQg.as<ublas::vector<double>>());

      auto ndQgMin = nd["Q_gen_min_setpt"];
      if (ndQgMin) bus.setQgMinSetpt(ndQgMin.as<ublas::vector<double>>());
      
      auto ndQgMax = nd["Q_gen_max_setpt"];
      if (ndQgMax) bus.setQgMaxSetpt(ndQgMax.as<ublas::vector<double>>());

      auto ndVMag = nd["V_mag_setpt"];
      if (ndVMag) bus.setVMagSetpt(ndVMag.as<ublas::vector<double>>());

      auto ndVMagMin = nd["V_mag_min_setpt"];
      if (ndVMagMin) bus.setVMagMinSetpt(ndVMagMin.as<ublas::vector<double>>());
      
      auto ndVMagMax = nd["V_mag_max_setpt"];
      if (ndVMagMax) bus.setVMagMaxSetpt(ndVMagMax.as<ublas::vector<double>>());

      auto ndVAng = nd["V_ang_setpt_rad"];
      if (ndVAng) bus.setVAngSetpt(ndVAng.as<ublas::vector<double>>());

      auto ndVAngMin = nd["V_ang_min_setpt_rad"];
      if (ndVAngMin) bus.setVAngMinSetpt(ndVAngMin.as<ublas::vector<double>>());
      
      auto ndVAngMax = nd["V_ang_max_setpt_rad"];
      if (ndVAngMax) bus.setVAngMaxSetpt(ndVAngMax.as<ublas::vector<double>>());
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
      ys_ = ublas::vector<Complex>(phases_.size(), czero);
      Ic_ = ublas::vector<Complex>(phases_.size(), czero);
      Sc_ = ublas::vector<Complex>(phases_.size(), czero);
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
   }

   void Bus::updateState(Time t0, Time t1)
   {
      for (int i = 0; i < phases_.size(); ++i)
      {
         ys_(i) = Ic_(i) = Sc_(i) = {0.0, 0.0};
      }
      for (const ZipToGroundBase * zip : zipsToGround_)
      {
         ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
   }

   Bus::Bus(const std::string & name, BusType type, const Phases & phases, const ublas::vector<Complex> & nominalV) :
      Component(name),
      type_(type),
      phases_(phases),
      nominalV_(nominalV),
      PgSetpt_(ublas::vector<double>(phases.size(), 0.0)),
      PgMinSetpt_(ublas::vector<double>(phases.size(), negInfinity)),
      PgMaxSetpt_(ublas::vector<double>(phases.size(), infinity)),
      QgSetpt_(ublas::vector<double>(phases.size(), 0.0)),
      QgMinSetpt_(ublas::vector<double>(phases.size(), negInfinity)),
      QgMaxSetpt_(ublas::vector<double>(phases.size(), infinity)),
      VMagSetpt_(ublas::vector<double>(phases.size(), 0.0)),
      VMagMinSetpt_(ublas::vector<double>(phases.size(), negInfinity)),
      VMagMaxSetpt_(ublas::vector<double>(phases.size(), infinity)),
      VAngSetpt_(ublas::vector<double>(phases.size(), 0.0)),
      VAngMinSetpt_(ublas::vector<double>(phases.size(), negInfinity)),
      VAngMaxSetpt_(ublas::vector<double>(phases.size(), infinity)),
      V_(nominalV),
      Sg_(ublas::vector<Complex>(phases.size(), czero)),
      ys_(ublas::vector<Complex>(phases.size(), czero)),
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
      needsUpdate().trigger();
   }
}
