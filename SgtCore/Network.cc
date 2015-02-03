#include "Network.h"

#include "PowerFlowNr.h"
#include "Zip.h"

#include <numeric>

namespace SmartGridToolbox
{
   Network::Network(const std::string& id, double PBase) :
      Component(id),
      PBase_(PBase)
   {
      // Empty.
   }

   void Network::addBus(BusPtr bus)
   {
      busMap_[bus->id()] = bus;
      busVec_.push_back(bus);
   }

   void Network::addBranch(BranchPtr branch, const std::string& bus0Id, const std::string& bus1Id)
   {
      auto bus0 = bus(bus0Id); 
      auto bus1 = bus(bus1Id); 
      if (bus0 == nullptr)
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus0Id << " was not found in the network." << std::endl;
      }
      if (bus1 == nullptr)
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus1Id << " was not found in the network." << std::endl;
      }
      branch->setBus0(bus0);
      branch->setBus1(bus1);
      branchMap_[branch->id()] = branch;
      branchVec_.push_back(branch);
   }
         
   void Network::addGen(GenPtr gen, const std::string& busId)
   {
      auto bus = this->bus(busId);
      if (bus != nullptr)
      {
         bus->addGen(gen);
      }
      else
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << busId << " was not found in the network." << std::endl;
      }
   }
         
   void Network::addZip(ZipPtr zip, const std::string& busId)
   {
      zipVec_.push_back(zip);
      auto bus = this->bus(busId);
      if (bus != nullptr)
      {
         bus->addZip(zip);
      }
      else
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << busId << " was not found in the network." << std::endl;
      }
   }

   void Network::solvePowerFlow()
   {
      SGT_DEBUG(Log().debug() << "Network : solving power flow." << std::endl);
      SGT_DEBUG(Log().debug() << *this);

      PowerFlowModel mod;
      for (const auto bus : busVec_)
      {
         bool enabledSv = bus->setpointChanged().enabled();
         bus->setpointChanged().setEnabled(false);
         BusType busTypeSv = bus->type();
         if (bus->type() != BusType::PQ)
         {
            if (!hasInServiceGen(*bus))
            {
               Log().warning() << "Bus " << bus->id() << " has type " << bus->type()
                               << ", but does not have any in service generators. Setting type to PQ." << std::endl;
               bus->setType(BusType::PQ);
            }
         }

         bus->applyVSetpoints();
         mod.addBus(bus->id(), bus->type(), bus->phases(), bus->YZip(), bus->IZip(), bus->SZip(), bus->JGen(),
                    bus->V(), bus->SGen() + bus->SZip());

         bus->setType(busTypeSv);
         bus->setpointChanged().setEnabled(enabledSv);
      }
      for (const auto branch : branchVec_)
      {
         if (branch->isInService())
         {
            // TODO: ignore like this, or add the branch with zero admittance?
            mod.addBranch(branch->bus0()->id(), branch->bus1()->id(),
                          branch->phases0(), branch->phases1(), branch->Y());
         }
      }
      mod.validate();

      PowerFlowNr solver(&mod);
      bool ok = solver.solve();
      if (!ok)
      {
         Log().warning() << "Couldn't solve power flow model" << std::endl;
      }

      for (const auto& busPair: mod.busses())
      {
         auto& busNr = *busPair.second;
         const auto bus = this->bus(busNr.id_);

         int nInService = bus->nInServiceGens(); 

         arma::Col<Complex> SGen = nInService > 0 
            ? (busNr.S_ - bus->SZip()) / nInService
            : arma::Col<Complex>(bus->phases().size(), arma::fill::zeros);
         // Note: we've already taken YZip and IZip explicitly into account, so this is correct.
         // KLUDGE: We're using a vector above, rather than "auto" (which gives some kind of expression type).
         // This is less efficient, but the latter gives errors in valgrind.
         // Also: regarding the nInService check, recall that if nInService = 0, the bus is treated as PQ for
         // the purpose of the solver.

         bus->setV(busNr.V_);
         switch (bus->type())
         {
            case BusType::SL:
               for (const auto& gen : bus->gens())
               {
                  if (gen->isInService())
                  {
                     gen->setInServiceS(SGen);
                  }
               }
               break;
            case BusType::PQ:
               break;
            case BusType::PV:
               for (const auto& gen : bus->gens())
               {
                  if (gen->isInService())
                  {
                     // Keep P for gens, distribute Q amongst all gens.
                     arma::Col<Complex> SNew(gen->S().size());
                     for (int i = 0; i < SNew.size(); ++i)
                     {
                        SNew[i] = Complex(gen->S()[i].real(), SGen[i].imag());
                     }
                     gen->setInServiceS(SNew);
                  }
               }
               break;
            default:
               Log().fatal() << "Bad bus type." << std::endl;
         }
      }
   }

   void Network::print(std::ostream& os) const
   {
      Component::print(os);
      StreamIndent _(os);
      os << "P_base: " << PBase_ << std::endl;
      for (auto bus : busVec_)
      {
         {
            os << "Bus: " << std::endl;
            StreamIndent _(os);
            os << *bus << std::endl;
         }
         {
            os << "Zips: " << std::endl;
            StreamIndent _(os);
            for (auto zip : bus->zips())
            {
               os << *zip << std::endl;
            }
         }
         {
            os << "Gens: " << std::endl;
            StreamIndent _(os);
            for (auto gen : bus->gens())
            {
               os << *gen << std::endl;
            }
         }
      }
      for (auto branch : branchVec_)
      {
         os << "Branch: " << std::endl;
         StreamIndent _1(os);
         os << *branch << std::endl;
         StreamIndent _2(os);
         os << "Bus 0 = " << branch->bus0()->id() << std::endl;
         os << "Bus 1 = " << branch->bus1()->id() << std::endl;
      }
   }

   bool Network::hasInServiceGen(const Bus& bus)
   {
      bool result = false;
      for (auto gen : bus.gens())
      {
         if (gen->isInService())
         {
            result = true;
            break;
         }
      }
      return result;
   }

   void Network::applySLSetpoints(Bus& bus)
   {
      arma::Col<Complex> VNew(bus.phases().size());
      for (int i = 0; i < bus.phases().size(); ++i)
      {
         VNew(i) = std::polar(bus.VMagSetpoint_(i), bus.VAngSetpoint_(i));
      }
      bus.setV(VNew);
   }

   void Network::applyPVSetpoints(Bus& bus)
   {
      arma::Col<Complex> VNew = bus.V();
      for (int i = 0; i < bus.phases().size(); ++i)
      {
         VNew(i) *= bus.VMagSetpoint_(i) / std::abs(bus.V_(i));
      }
      bus.setV(VNew);
   }
}
