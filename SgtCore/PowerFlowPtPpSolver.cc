#include "PowerFlowPtPpSolver.h"

#include "Network.h"
#include "CommonBranch.h"
#include "Stopwatch.h"

#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>

#include <set>

namespace Sgt
{
   std::unique_ptr<Net> sgt2PowerTools(const Sgt::Network& sgtNw)
   {
      std::unique_ptr<Net> net(new Net);

      net->bMVA = sgtNw.PBase();
      for (auto& bus : sgtNw.busses())
      {
         std::string busId = bus->id();

         Complex PLoad = 0;
         Complex yShunt = 0;
         for (auto zip : bus->zips())
         {
            PLoad += -sgtNw.S2Pu(zip->SConst()[0]);
            yShunt += sgtNw.Y2Pu(zip->YConst()[0], bus->VBase());
         }
         double VMin = sgtNw.V2Pu(bus->VMagMin(), bus->VBase());
         double VMax = sgtNw.V2Pu(bus->VMagMax(), bus->VBase());
         double kVBase = bus->VBase();

         Node* node = new Node(busId, PLoad.real(), PLoad.imag(), yShunt.real(), yShunt.imag(),
               VMin, VMax, kVBase, 1);
         net->add_node(node);
         
         for (const auto& gen : bus->gens())
         {
            double PMin = sgtNw.S2Pu(gen->PMin());
            double PMax = sgtNw.S2Pu(gen->PMax());
            double QMin = sgtNw.S2Pu(gen->QMin());
            double QMax = sgtNw.S2Pu(gen->QMax());

            Gen* g = new Gen(node, to_string(node->_gen.size()), PMin, PMax, QMin, QMax);
            g->_active = gen->isInService();

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

         arc->tbound.min = cBranch->angMin();
         arc->tbound.max = cBranch->angMax();

         arc->limit = lim;
            
         arc->status = branch->isInService() ? 1 : 0;

         net->add_arc(arc);
      }
      return net;
   }

   void powerTools2Sgt(const Net& ptNetw, Sgt::Network& sgtNw)
   {
      for (auto node: ptNetw.nodes)
      {
         auto sgtBus = sgtNw.bus(node->_name);
         assert(sgtBus->gens().size() == node->_gen.size());

         Complex VSolPu(node->vr.get_value(), node->vi.get_value());
         Complex VSol = sgtNw.pu2V<Complex>(VSolPu, sgtBus->VBase());

         sgtBus->setV({VSol});

         Complex SLoadUnservedSolPu(-node->plv.get_value(), -node->qlv.get_value());
         Complex SLoadUnservedSol = sgtNw.pu2S(SLoadUnservedSolPu);
         if (std::abs(SLoadUnservedSol) > 1e-6)
         {
            Log().warning() << "Unserved load at bus " << sgtBus->id() << " is " << SLoadUnservedSol << std::endl;
         }
         
         sgtBus->setSZipUnserved({SLoadUnservedSol});
         sgtBus->setSGenUnserved(arma::Col<Complex>(sgtBus->phases().size(), arma::fill::zeros));

         int nGen = node->_gen.size();
         int nSgtGen = sgtBus->gens().size();
         assert(nGen == nSgtGen);
         for (int i = 0; i < nGen; ++i)
         {
            // Order of gens should be same in Sgt and Pt.
            auto gen = node->_gen[i];
            auto sgtGen = std::dynamic_pointer_cast<Sgt::GenericGen>(sgtBus->gens()[i]);
            Complex SGenSolPu(gen->pg.get_value(), gen->qg.get_value());
            Complex SGenSol = sgtNw.pu2S(SGenSolPu);
            sgtGen->setInServiceS({SGenSol});
         }
      }
   }

   void printNetw(const Net& net)
   {
      Log().message() << "Nodes-----------------" << std::endl;
      for (const auto node : net.nodes)
      {
         node->print();
      }
      Log().message() << "Arcs------------------" << std::endl;
      for (const auto arc : net.arcs)
      {
         arc->print();
      }
      Log().message() << "Gens------------------" << std::endl;
      for (const auto gen : net.gens)
      {
         gen->print();
      }
      Log().message() << "Done------------------" << std::endl;
   }

   bool PowerFlowPtPpSolver::solve(Network* netw)
   {
      Stopwatch stopwatchPre;
      Stopwatch stopwatchSolve;
      Stopwatch stopwatchPost;
      stopwatchPre.start();
      auto net = sgt2PowerTools(*netw);
      stopwatchPre.stop();
      // printNetw(*net);
      stopwatchSolve.start();
      PowerModel pModel(ACRECT_, net.get());
      pModel.min_cost_load();
      stopwatchSolve.stop();
      stopwatchPost.start();
      // printNetw(*net);
      powerTools2Sgt(*net, *netw);
      stopwatchPost.stop();
      Log().message() << "PowerFlowPtPpSolver:" << std::endl;
      {
         LogIndent _;
         Log().message() << "Preprocessing time  = " << stopwatchPre.seconds() << std::endl;
         Log().message() << "Solve time          = " << stopwatchSolve.seconds() << std::endl;
         Log().message() << "Postprocessing time = " << stopwatchPost.seconds() << std::endl;
         Log().message() << "Total time          = " 
                   << stopwatchPre.seconds() + stopwatchSolve.seconds() + stopwatchPost.seconds() << std::endl;
      }
      return true;
   }
}
