#include "Network.h"

#include "PowerFlowNr.h"

namespace SmartGridToolbox
{
   Network::Network(const std::string& id, double PBase) :
      Component(id),
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
      for (const std::shared_ptr<Bus>& bus : busVec_)
      {
         solver.addBus(bus->id(), bus->type(), bus->phases(), bus->V(), bus->YZip(), bus->IZip(), 
               bus->SZip() + bus->SGen());
      }
      for (const std::shared_ptr<Branch>& branch : branchVec_)
      {
         solver.addBranch(branch->bus0().id(), branch->bus1().id(), branch->phases0(), branch->phases1(),
               branch->Y());
      }

      solver.validate();

      bool ok = solver.solve();

      if (ok)
      {
         for (const auto& busPair: solver.busses())
         {
            const std::shared_ptr<Bus>& bus = this->bus(busPair.second->id_);
            // Push the state back onto bus. We don't want to trigger any events.
            bus->setV(busPair.second->V_);
            auto SGen = busPair.second->S_ - bus->SZip();
            // TODO propagate back to the bus.
            // switch (bus->busType())
            // {
               // case BusType::PV:
            // }
         }
      }
   }
}
