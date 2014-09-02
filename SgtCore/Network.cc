#include "Network.h"

#include "PowerFlowNr.h"

#include <numeric>

namespace SmartGridToolbox
{
   ublas::vector<Complex> Node::YZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const std::shared_ptr<Zip>& zip) {return tot + zip->YConst();});
   }

   ublas::vector<Complex> Node::IZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const std::shared_ptr<Zip>& zip) {return tot + zip->IConst();});
   }

   ublas::vector<Complex> Node::SZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const std::shared_ptr<Zip>& zip) {return tot + zip->SConst();});
   }
   
   ublas::vector<Complex> Node::SGen() const
   {
      return std::accumulate(gens_.begin(), gens_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const std::shared_ptr<GenInterface>& gen) {return tot + gen->S();});
   }

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
      for (auto nd : nodeVec_)
      {
         os << nd->bus() << std::endl;
         os << "Zips: " << std::endl;
         {
            IndentingOStreamBuf _(os);
            for (auto zip : nd->zips())
            {
               os << *zip << std::endl;
            }
         }
         os << "Gens: " << std::endl;
         {
            IndentingOStreamBuf _(os);
            for (auto gen : nd->gens())
            {
               os << *gen << std::endl;
            }
         }
      }
      for (auto arc : arcVec_)
      {
         os << arc->branch() << std::endl;
         os << "Bus 0 = " << arc->node0()->bus()->id() << std::endl;
         os << "Bus 1 = " << arc->node0()->bus()->id() << std::endl;
      }
   }

   void Network::solvePowerFlow()
   {
      SGT_DEBUG(debug() << "Network : solving power flow." << std::endl);
      SGT_DEBUG(debug() << *this);
      PowerFlowNr solver;
      solver.reset();
      for (const auto nd : nodeVec_)
      {
         auto bus = nd->bus();
         solver.addBus(bus->id(), bus->type(), bus->phases(), bus->VNom(), nd->YZip(), nd->IZip(), 
               nd->SZip() + nd->SGen());
      }
      for (const auto arc : arcVec_)
      {
         auto branch = arc->branch();
         solver.addBranch(arc->node0()->bus()->id(), arc->node1()->bus()->id(),
               branch->phases0(), branch->phases1(), branch->Y());
      }

      solver.validate();

      bool ok = solver.solve();

      if (ok)
      {
         for (const auto& busPair: solver.busses())
         {
            auto& busNr = *busPair.second;
            const auto nd = this->node(busNr.id_);
            auto SGen = (busNr.S_ - nd->SZip()) / double(nd->gens().size());
            // Note: we've already taken YZip and IZip explicitly into account, so this is correct.
           
            auto bus = nd->bus();
            bus->setV(busNr.V_);
            switch (bus->type())
            {
               case BusType::SL:
                  for (auto gen : nd->gens())
                  {
                     gen->setS(SGen);
                  }
                  break;
               case BusType::PQ:
                  break;
               case BusType::PV:
                  for (auto gen : nd->gens())
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
