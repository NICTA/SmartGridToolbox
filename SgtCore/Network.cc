#include "Network.h"

#include "PowerFlowNr.h"

namespace SmartGridToolbox
{
   Network::Network(const std::string& id, Model& model, double PBase) :
      Component(id),
      model_(&model),
      PBase_(PBase)
   {
      // Empty.
   }

   void Network::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "P_base: " << PBase_ << std::endl;
      for (auto& bus : busVec_)
      {
         os << *bus << std::endl;
      }
      for (auto& branch : branchVec_)
      {
         os << *branch << std::endl;
      }
   }

   void Network::solvePowerFlow()
   {
      SGT_DEBUG(debug() << "Network : solving power flow." << std::endl);
      SGT_DEBUG(debug() << *this);
      PowerFlowNr solver;
      solver.reset();
      for (const SimBus* bus : busVec_)
      {
         solver.addBus(bus->name(), bus->type(), bus->phases(), bus->V(), bus->Ys(), bus->Ic(), bus->STot());
      }
      for (const SimBranch* branch : branchVec_)
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
            SimBus* bus = findBus(busPair.second->id_);

            // Push the state back onto bus. We don't want to trigger any events.
            bus->setV(busPair.second->V_);
            bus->setSg(busPair.second->S_ - bus->Sc());
         }
      }
   }
}
