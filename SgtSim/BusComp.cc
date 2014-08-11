#include "BusComp.h"

#include "Model.h"
#include "NetworkComp.h"
#include "ZipBase.h"

namespace SmartGridToolbox
{
   BusComp::BusComp(const std::string& id, Phases phases, const ublas::vector<Complex> & VNom, double VBase) :
      Component(name),
      Bus(name, phases, VNom, VBase),
      changed_("BusComp " + name + " setpoint changed")
   {
      // Empty.
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
