#include "Network.h"

#include "PowerFlowNr.h"
#include "Zip.h"

#include <numeric>

namespace SmartGridToolbox
{
   ublas::vector<Complex> Node::YZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const ZipPtr& zip) 
            {return tot + zip->YConst();});
   }

   ublas::vector<Complex> Node::IZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const ZipPtr& zip)
            {return tot + zip->IConst();});
   }

   ublas::vector<Complex> Node::SZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const ZipPtr& zip)
            {return tot + zip->SConst();});
   }
   
   ublas::vector<Complex> Node::SGen() const
   {
      return std::accumulate(gens_.begin(), gens_.end(), ublas::vector<Complex>(bus_->phases().size(), czero),
            [] (ublas::vector<Complex> & tot, const GenPtr& gen) {return tot + gen->S();});
   }

   Network::Network(const std::string& id, double PBase) :
      Component(id),
      PBase_(PBase)
   {
      // Empty.
   }
         
   void Network::addNode(BusPtr bus)
   {
      auto nd = NodePtr(new Node(bus)); 
      nodeMap_[bus->id()] = nd;
      nodeVec_.push_back(nd);
   }

   void Network::addArc(BranchPtr branch, const std::string& bus0Id, const std::string& bus1Id)
   {
      auto nd0 = node(bus0Id); 
      auto nd1 = node(bus1Id); 
      if (nd0 == nullptr)
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus0Id << " was not found in the network." << std::endl;
      }
      if (nd1 == nullptr)
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus1Id << " was not found in the network." << std::endl;
      }
      auto arc = ArcPtr(new Arc(branch, nd0, nd1));
      arcMap_[branch->id()] = arc;
      arcVec_.push_back(arc);
   }
         
   void Network::addGen(GenPtr gen, const std::string& busId)
   {
      genMap_[gen->id()] = gen;
      genVec_.push_back(gen);
      auto busNd = node(busId);
      if (busNd != nullptr)
      {
         busNd->gens_.push_back(gen);
      }
      else
      {
         Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << busId << " was not found in the network." << std::endl;
      }
   }
         
   void Network::addZip(ZipPtr zip, const std::string& busId)
   {
      zipMap_[zip->id()] = zip;
      zipVec_.push_back(zip);
      auto busNd = node(busId);
      if (busNd != nullptr)
      {
         busNd->zips_.push_back(zip);
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
      for (const auto nd : nodeVec_)
      {
         auto bus = nd->bus();
         mod.addBus(bus->id(), bus->type(), bus->phases(), bus->VNom(), nd->YZip(), nd->IZip(), 
               nd->SZip() + nd->SGen());
      }
      for (const auto arc : arcVec_)
      {
         auto branch = arc->branch();
         mod.addBranch(arc->node0()->bus()->id(), arc->node1()->bus()->id(),
               branch->phases0(), branch->phases1(), branch->Y());
      }

      PowerFlowNr solver(&mod);

      solver.validate();

      bool ok = solver.solve();

      if (ok)
      {
         for (const auto& busPair: mod.busses())
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
                  Log().fatal() << "Bad bus type." << std::endl;
            }
         }
      }
   }

   void Network::print(std::ostream& os) const
   {
      Component::print(os);
      StreamIndent _(os);
      os << "P_base: " << PBase_ << std::endl;
      for (auto nd : nodeVec_)
      {
         {
            os << "Bus: " << std::endl;
            StreamIndent _(os);
            os << *nd->bus() << std::endl;
         }
         {
            os << "Zips: " << std::endl;
            StreamIndent _(os);
            for (auto zip : nd->zips())
            {
               os << *zip << std::endl;
            }
         }
         {
            os << "Gens: " << std::endl;
            StreamIndent _(os);
            for (auto gen : nd->gens())
            {
               os << *gen << std::endl;
            }
         }
      }
      for (auto arc : arcVec_)
      {
         os << "Branch: " << std::endl;
         StreamIndent _1(os);
         os << *arc->branch() << std::endl;
         StreamIndent _2(os);
         os << "Bus 0 = " << arc->node0()->bus()->id() << std::endl;
         os << "Bus 1 = " << arc->node0()->bus()->id() << std::endl;
      }
   }
}
