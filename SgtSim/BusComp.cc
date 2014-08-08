#include <SgtSim/BusComp.h>
#include <SgtSim/Model.h>
#include <SgtSim/NetworkComp.h>
#include <SgtSim/ZipBase.h>

namespace SmartGridToolbox
{
   void BusComp::initializeState()
   {
      Ys_ = ublas::vector<Complex>(phases_.size(), czero);
      Ic_ = ublas::vector<Complex>(phases_.size(), czero);
      Sc_ = ublas::vector<Complex>(phases_.size(), czero);
      for (const ZipBase* zip : zips_)
      {
         Ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
   }

   void BusComp::updateState(Time t)
   {
      applySetpoints();
      for (int i = 0; i < phases_.size(); ++i)
      {
         Ys_(i) = Ic_(i) = Sc_(i) = {0.0, 0.0};
      }
      for (const ZipBase* zip : zips_)
      {
         Ys_ += zip->Y();
         Ic_ += zip->I(); // Injection.
         Sc_ += zip->S(); // Injection.
      }
   }
   
   BusComp::BusComp(const std::string& name, BusType type, const Phases& phases,
         const ublas::vector<Complex>& nominalV) : Component(name),
      type_(type),
      phases_(phases),
      nominalV_(nominalV),
      PgSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      VMagSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      VAngSetpoint_(ublas::vector<double>(phases.size(), 0.0)),
      V_(nominalV),
      Sg_(ublas::vector<Complex>(phases.size(), czero)),
      Ys_(ublas::vector<Complex>(phases.size(), czero)),
      Ic_(ublas::vector<Complex>(phases.size(), czero)),
      Sc_(ublas::vector<Complex>(phases.size(), czero)),
      changed_("BusComp " + name + " setpoint changed")
   {
      assert(nominalV_.size() == phases_.size());
      for (int i = 0; i < phases_.size(); ++i)
      {
         VMagSetpoint_(i) = std::abs(nominalV(i));
         VAngSetpoint_(i) = std::arg(nominalV(i));
      }
   }

   void BusComp::setPgSetpoint(const ublas::vector<double>& PgSetpoint)
   {
      if (PgSetpoint.size() != phases_.size())
      {
         error() << "BusComp::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      PgSetpoint_ = PgSetpoint;
      changed().trigger();
   }

   void BusComp::setVMagSetpoint(const ublas::vector<double>& VMagSetpoint)
   {
      if (VMagSetpoint.size() != phases_.size())
      {
         error() << "BusComp::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VMagSetpoint_ = VMagSetpoint;
      changed().trigger();
   }

   void BusComp::setVAngSetpoint(const ublas::vector<double>& VAngSetpoint)
   {
      if (VAngSetpoint.size() != phases_.size())
      {
         error() << "BusComp::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VAngSetpoint_ = VAngSetpoint;
      changed().trigger();
   }

   void BusComp::applySetpoints()
   {
      switch (type())
      {
         case BusType::SL :
            {
               ublas::vector<Complex> newV(phases().size());
               for (int i = 0; i < phases().size(); ++i)
               {
                  newV(i) = polar(VMagSetpoint()(i), VAngSetpoint()(i));
               }
               setV(newV);
            }
            break;
         case BusType::PV :
            {
               ublas::vector<Complex> newSg(phases().size());
               ublas::vector<Complex> newV(phases().size());
               for (int i = 0; i < phases().size(); ++i)
               {
                  newSg(i) = {PgSetpoint()(i), Sg()(i).imag()};
                  newV(i) = VMagSetpoint()(i)*V()(i)/abs(V()(i));
               }
               setSg(newSg);
               setV(newV);
            }
            break;
         default :
            ;
      }
   }

   void BusComp::addZip(ZipBase& zip)
   {
      dependsOn(zip);
      zips_.push_back(&zip);
      zip.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger BusComp " + name() + " needsUpdate.");
      zip.didUpdate().addAction([this](){changed().trigger();},
            "Trigger BusComp " + name() + " changed.");
      // TODO: this will recalculate all zips. Efficiency?
      changed().trigger();
   }
}
