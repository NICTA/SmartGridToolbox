#include "PowerFlowPtPpSolver.h"

#include "Network.h"
#include "CommonBranch.h"

#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>

#include <set>

namespace SmartGridToolbox
{
   std::unique_ptr<Net> sgt2PowerTools(const SmartGridToolbox::Network& sgtNw)
   {
      std::unique_ptr<Net> net(new Net);

      net->bMVA = sgtNw.PBase();
      for (auto& bus : sgtNw.busses())
      {
         const auto& zip = *bus->zips().begin();

         std::string busId = bus->id();
         Complex PLoad = -sgtNw.S2Pu(zip->SConst()[0]);
         Complex yShunt = sgtNw.Y2Pu(zip->YConst()[0], bus->VBase());
         double VMin = sgtNw.V2Pu(bus->VMagMin(), bus->VBase());
         double VMax = sgtNw.V2Pu(bus->VMagMax(), bus->VBase());
         double kVBase = bus->VBase();

         Node* node = new Node(busId, PLoad.real(), PLoad.imag(), yShunt.real(), yShunt.imag(),
               VMin, VMax, kVBase, 1);
         net->add_node(node);
         
         for (const auto& gen : bus->gens())
         {
            // if (!gen->status()) continue;
            double PMin = sgtNw.S2Pu(gen->PMin());
            double PMax = sgtNw.S2Pu(gen->PMax());
            double QMin = sgtNw.S2Pu(gen->QMin());
            double QMax = sgtNw.S2Pu(gen->QMax());

            Gen* g = new Gen(node, to_string(node->_gen.size()), PMin, PMax, QMin, QMax);

            double c0 = gen->c0();
            double c1 = gen->c1();
            double c2 = gen->c2();
            g->set_costs(c0, c1, c2);
            
            net->gens.push_back(g);
            node->_gen.push_back(g);
         }
      }

      for (const auto& branch : sgtNw.branches())
      {
         // if (!branch->status()) continue;
         auto cBranch = std::dynamic_pointer_cast<CommonBranch>(branch);
         auto bus0 = branch->bus0();
         auto bus1 = branch->bus1();

         std::string id = cBranch->id();
         std::string bus0Id = bus0->id();
         std::string bus1Id = bus1->id();

         auto arc = new Arc(id);
         arc->src = net->get_node(bus0Id);
         arc->dest = net->get_node(bus1Id);
         arc->connect();

         Complex YSeries = sgtNw.Y2Pu(cBranch->YSeries(), bus1->VBase());
         Complex ZSeries = sgtNw.Z2Pu(1.0/cBranch->YSeries(), bus1->VBase());
         Complex yShunt = sgtNw.Y2Pu(cBranch->YShunt(), bus1->VBase());
         double lim = sgtNw.S2Pu(cBranch->rateA());
         Complex tap = cBranch->tapRatio()*bus1->VBase()/bus0->VBase(); // Off nominal tap ratio.
         
         arc->g = YSeries.real();
         arc->b = YSeries.imag();

         arc->r = ZSeries.real();
         arc->x = ZSeries.imag();

         arc->tr = std::abs(tap);
         arc->as = std::arg(tap);

         assert(abs(yShunt.real()) < 1e-9);
         arc->ch = yShunt.imag();

         arc->cc = arc->tr*cos(arc->as);
         arc->dd = arc->tr*sin(arc->as);

         // TODO arc->tbound

         arc->limit = lim;

         net->add_arc(arc);
      }
      return net;
   }

   void powerTools2Sgt(const Net& ptNetw, SmartGridToolbox::Network& sgtNetw)
   {
      for (auto node: ptNetw.nodes)
      {
         auto sgtBus = sgtNetw.bus(node->_name);
         sgtBus->setV({sgtNetw.pu2V<Complex>({node->vr.get_value(), node->vi.get_value()}, sgtBus->VBase())});
         assert(sgtBus->gens().size() == node->_gen.size()); // TODO: remove restriction.
         int nGen = node->_gen.size();
         assert(nGen <= 1); // TODO: remove restriction.
         if (nGen == 1)
         {
            auto gen = node->_gen[0];
            auto sgtGen = std::dynamic_pointer_cast<SmartGridToolbox::GenericGen>(sgtBus->gens()[0]);
            sgtGen->setInServiceS({sgtNetw.pu2S<Complex>({gen->pg.get_value(), gen->qg.get_value()})});
         }
      }
   }

   void printNetw(const Net& net)
   {
      for (const auto node : net.nodes)
      {
         std::cout << "Nodes:" << std::endl;
         std::cout 
            << node->_name << ": " << node->v.get_value() << " " 
            << node->vr.get_value() << " " << node->vi.get_value() << " " << std::endl;
         std::cout << "Arcs:" << std::endl;
      }
   }

   bool PowerFlowPtPpSolver::solve(Network* netw)
   {
      auto net = sgt2PowerTools(*netw);
      // printNetw(*net);
      PowerModel pModel(ACRECT, net.get());
      pModel.min_cost();
      // printNetw(*net);
      powerTools2Sgt(*net, *netw);
      return true;
   }
}
