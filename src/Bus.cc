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

      auto ndPg = nd["P_gen_setpoint"];
      if (ndPg) bus.setPgSetpoint(ndPg.as<ublas::vector<double>>());

      auto ndPgMin = nd["P_gen_min_setpoint"];
      if (ndPgMin) bus.setPgMinSetpoint(ndPgMin.as<ublas::vector<double>>());
      
      auto ndPgMax = nd["P_gen_max_setpoint"];
      if (ndPgMax) bus.setPgMaxSetpoint(ndPgMax.as<ublas::vector<double>>());

      auto ndQg = nd["Q_gen_setpoint"];
      if (ndQg) bus.setQgSetpoint(ndQg.as<ublas::vector<double>>());

      auto ndQgMin = nd["Q_gen_min_setpoint"];
      if (ndQgMin) bus.setQgMinSetpoint(ndQgMin.as<ublas::vector<double>>());
      
      auto ndQgMax = nd["Q_gen_max_setpoint"];
      if (ndQgMax) bus.setQgMaxSetpoint(ndQgMax.as<ublas::vector<double>>());

      auto ndVMag = nd["V_mag_setpoint"];
      if (ndVMag) bus.setVMagSetpoint(ndVMag.as<ublas::vector<double>>());

      auto ndVMagMin = nd["V_mag_min_setpoint"];
      if (ndVMagMin) bus.setVMagMinSetpoint(ndVMagMin.as<ublas::vector<double>>());
      
      auto ndVMagMax = nd["V_mag_max_setpoint"];
      if (ndVMagMax) bus.setVMagMaxSetpoint(ndVMagMax.as<ublas::vector<double>>());

      auto ndVAng = nd["V_ang_setpoint_rad"];
      if (ndVAng) bus.setVAngSetpoint(ndVAng.as<ublas::vector<double>>());

      auto ndVAngMin = nd["V_ang_min_setpoint_rad"];
      if (ndVAngMin) bus.setVAngMinSetpoint(ndVAngMin.as<ublas::vector<double>>());
      
      auto ndVAngMax = nd["V_ang_max_setpoint_rad"];
      if (ndVAngMax) bus.setVAngMaxSetpoint(ndVAngMax.as<ublas::vector<double>>());
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
      PgSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      PgMinSetpoint_(ublas::vector<double>(phases.size(), negInfinity)),
      PgMaxSetpoint_(ublas::vector<double>(phases.size(), infinity)),
      QgSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      QgMinSetpoint_(ublas::vector<double>(phases.size(), negInfinity)),
      QgMaxSetpoint_(ublas::vector<double>(phases.size(), infinity)),
      VMagSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      VMagMinSetpoint_(ublas::vector<double>(phases.size(), negInfinity)),
      VMagMaxSetpoint_(ublas::vector<double>(phases.size(), infinity)),
      VAngSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      VAngMinSetpoint_(ublas::vector<double>(phases.size(), negInfinity)),
      VAngMaxSetpoint_(ublas::vector<double>(phases.size(), infinity)),
      V_(nominalV),
      Sg_(ublas::vector<Complex>(phases.size(), czero)),
      ys_(ublas::vector<Complex>(phases.size(), czero)),
      Ic_(ublas::vector<Complex>(phases.size(), czero)),
      Sc_(ublas::vector<Complex>(phases.size(), czero))
   {
      assert(nominalV_.size() == phases_.size());
   }
         
   void Bus::setPgSetpoint(const ublas::vector<double> & PgSetpoint)
   {
      if (PgSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      PgSetpoint_ = PgSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setPgMinSetpoint(const ublas::vector<double> & PgMinSetpoint)
   {
      if (PgMinSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      PgMinSetpoint_ = PgMinSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setPgMaxSetpoint(const ublas::vector<double> & PgMaxSetpoint)
   {
      if (PgMaxSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      PgMaxSetpoint_ = PgMaxSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setQgSetpoint(const ublas::vector<double> & QgSetpoint)
   {
      if (QgSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      QgSetpoint_ = QgSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setQgMinSetpoint(const ublas::vector<double> & QgMinSetpoint)
   {
      if (QgMinSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      QgMinSetpoint_ = QgMinSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setQgMaxSetpoint(const ublas::vector<double> & QgMaxSetpoint)
   {
      if (QgMaxSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      QgMaxSetpoint_ = QgMaxSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVMagSetpoint(const ublas::vector<double> & VMagSetpoint)
   {
      if (VMagSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VMagSetpoint_ = VMagSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVMagMinSetpoint(const ublas::vector<double> & VMagMinSetpoint)
   {
      if (VMagMinSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VMagMinSetpoint_ = VMagMinSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVMagMaxSetpoint(const ublas::vector<double> & VMagMaxSetpoint)
   {
      if (VMagMaxSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VMagMaxSetpoint_ = VMagMaxSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVAngSetpoint(const ublas::vector<double> & VAngSetpoint)
   {
      if (VAngSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VAngSetpoint_ = VAngSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVAngMinSetpoint(const ublas::vector<double> & VAngMinSetpoint)
   {
      if (VAngMinSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VAngMinSetpoint_ = VAngMinSetpoint;
      needsUpdate().trigger();
   }
         
   void Bus::setVAngMaxSetpoint(const ublas::vector<double> & VAngMaxSetpoint)
   {
      if (VAngMaxSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VAngMaxSetpoint_ = VAngMaxSetpoint;
      needsUpdate().trigger();
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
