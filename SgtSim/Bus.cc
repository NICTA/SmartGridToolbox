#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/ZipBase.h>

namespace SmartGridToolbox
{
   void Bus::initializeState()
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

   void Bus::updateState(Time t)
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
   
   Bus::Bus(const std::string& name, BusType type, const Phases& phases, const ublas::vector<Complex>& nominalV) :
      Component(name),
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
      changed_("Bus " + name + " setpoint changed")
   {
      assert(nominalV_.size() == phases_.size());
      for (int i = 0; i < phases_.size(); ++i)
      {
         VMagSetpoint_(i) = std::abs(nominalV(i));
         VAngSetpoint_(i) = std::arg(nominalV(i));
      }
   }

   void Bus::setPgSetpoint(const ublas::vector<double>& PgSetpoint)
   {
      if (PgSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      PgSetpoint_ = PgSetpoint;
      changed().trigger();
   }

   void Bus::setVMagSetpoint(const ublas::vector<double>& VMagSetpoint)
   {
      if (VMagSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VMagSetpoint_ = VMagSetpoint;
      changed().trigger();
   }

   void Bus::setVAngSetpoint(const ublas::vector<double>& VAngSetpoint)
   {
      if (VAngSetpoint.size() != phases_.size())
      {
         error() << "Bus::setPgSetpoint : setpoint should have same size as the number of phases." << std::endl;
      }
      VAngSetpoint_ = VAngSetpoint;
      changed().trigger();
   }

   void Bus::applySetpoints()
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

   void Bus::addZip(ZipBase& zip)
   {
      dependsOn(zip);
      zips_.push_back(&zip);
      zip.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger Bus " + name() + " needsUpdate.");
      zip.didUpdate().addAction([this](){changed().trigger();},
            "Trigger Bus " + name() + " changed.");
      // TODO: this will recalculate all zips. Efficiency?
      changed().trigger();
   }
}
