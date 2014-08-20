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
         solver.addBus(bus->id(), bus->type(), bus->phases(), bus->VNom(), bus->YZip(), bus->IZip(), 
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
            auto& busNr = *busPair.second;
            const std::shared_ptr<Bus>& bus = this->bus(busNr.id_);
            auto SGen = (busNr.S_ - bus->SZip()) / double(bus->gens().size());
            // Note: we've already taken YZip and IZip explicitly into account, so this is correct.
            
            bus->setV(busNr.V_);
            switch (bus->type())
            {
               case BusType::SL:
                  for (auto gen : bus->gens())
                  {
                     gen->setS(SGen);
                  }
                  break;
               case BusType::PQ:
                  break;
               case BusType::PV:
                  for (auto gen : bus->gens())
                  {
                     // Keep P for gens, distribute Q amongst all gens.
                     ublas::vector<Complex> SNew(gen->S().size());
                     for (int i = 0; i < SNew.size(); ++i)
                     {
                        SNew[i] = Complex(gen->S()[i].real(), SGen[i].imag());
                     }
                     gen->setS(SNew);
                  }
                  break;
               default:
                  error() << "Bad bus type." << std::endl;
                  abort();
            }
         }
      }
   }
}
