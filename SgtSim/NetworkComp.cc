#include <SgtSim/NetworkComp.h>
#include <SgtSim/BusComp.h>
#include <SgtSim/BranchComp.h>
#include <SgtSim/Model.h>

#include <SgtCore/PowerFlowNr.h>

#include <iostream>

namespace SmartGridToolbox
{
   void NetworkComp::updateState(Time t)
   {
      SGT_DEBUG(debug() << "NetworkComp : update state." << std::endl);
      solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }

   void NetworkComp::addBus(BusComp& bus)
   {
      dependsOn(bus);
      busVec_.push_back(&bus);
      busMap_[bus.name()] = &bus;
      bus.changed().addAction([this](){needsUpdate().trigger();},
            "Trigger NetworkComp " + name() + " needs update");
   }

   void NetworkComp::addBranch(BranchComp& branch)
   {
      dependsOn(branch);
      branchVec_.push_back(&branch);
      branch.changed().addAction([this](){needsUpdate().trigger();},
            "Trigger NetworkComp " + name() + " needs update");
   }

   void NetworkComp::solvePowerFlow()
   {
      SGT_DEBUG(debug() << "NetworkComp : solving power flow." << std::endl);
      SGT_DEBUG(debug() << *this);
      PowerFlowNr solver;
      solver.reset();
      for (const BusComp* bus : busVec_)
      {
         solver.addBus(bus->name(), bus->type(), bus->phases(), bus->V(), bus->Ys(), bus->Ic(), bus->STot());
      }
      for (const BranchComp* branch : branchVec_)
      {
         solver.addBranch(branch->bus0().name(), branch->bus1().name(), branch->phases0(),
                          branch->phases1(), branch->Y());
      }

      solver.validate();

      bool ok = solver.solve();

      if (ok)
      {
         for (const auto& busPair: solver.busses())
         {
            BusComp* bus = findBus(busPair.second->id_);

            // Push the state back onto bus. We don't want to trigger any events.
            bus->setV(busPair.second->V_);
            bus->setSg(busPair.second->S_ - bus->Sc());
         }
      }
   }

   std::ostream& operator<<(std::ostream& os, const NetworkComp& nw)
   {
      os << "NetworkComp: " << nw.name() << std::endl;
      os << "\tBusses:" << std::endl;
      for (const BusComp* bus : nw.busVec_)
      {
         ublas::vector<double> VMag(bus->V().size());
         for (int i = 0; i < bus->V().size(); ++i) VMag(i) = abs(bus->V()(i));

         os << "\t\tBus:" << std::endl;
         os << "\t\t\tName   : " << bus->name() << std::endl;
         os << "\t\t\tType   : " << bus->type() << std::endl;
         os << "\t\t\tPhases : " << bus->phases() << std::endl;
         os << "\t\t\tV      : " << bus->V() << std::endl;
         os << "\t\t\t|V|    : " << VMag << std::endl;
         os << "\t\t\tYs     : " << bus->Ys() << std::endl;
         os << "\t\t\tIc     : " << bus->Ic() << std::endl;
         os << "\t\t\tSc     : " << bus->Sc() << std::endl;
         os << "\t\t\tSg     : " << bus->Sg() << std::endl;
         os << "\t\t\tSTot   : " << bus->STot() << std::endl;
      }
      os << "\tBranches:" << std::endl;
      for (const BranchComp* branch : nw.branchVec_)
      {
         os << "\t\tBranch:" << std::endl;
         os << "\t\t\tBus names  : "
            << branch->bus0().name() << " " << branch->bus1().name() << std::endl;
         os << "\t\t\tBus phases : " << branch->phases0() << " " << branch->phases1() << std::endl;
         os << "\t\t\tY          :" << std::endl;
         for (int i = 0; i < branch->Y().size1(); ++i)
         {
            os << "\t\t\t\t" << std::setw(16) << row(branch->Y(), i) << std::endl;
         }
      }
      return os;
   }
}
