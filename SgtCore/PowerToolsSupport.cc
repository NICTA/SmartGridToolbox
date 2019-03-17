// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PowerToolsSupport.h"

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
        for (auto& bus : sgtNw.buses())
        {
            std::string busId = bus->id();

            Complex PLoad = sgtNw.S2Pu(bus->SConst()(0));
            Complex yShunt = sgtNw.Y2Pu(bus->YConst()(0), bus->VBase());
            Complex V = sgtNw.V2Pu(bus->V()(0), bus->VBase());
            double VMin = sgtNw.V2Pu(bus->VMagMin(), bus->VBase());
            double VMax = sgtNw.V2Pu(bus->VMagMax(), bus->VBase());
            double kVBase = bus->VBase();

            Node* node = new Node(busId, PLoad.real(), PLoad.imag(), yShunt.real(), yShunt.imag(),
                                  VMin, VMax, kVBase, 1);
            node->vs = std::abs(V);
            net->add_node(node);

            for (const auto& gen : bus->gens())
            {
                double PMin = sgtNw.S2Pu(gen->PMin());
                double PMax = sgtNw.S2Pu(gen->PMax());
                double QMin = sgtNw.S2Pu(gen->QMin());
                double QMax = sgtNw.S2Pu(gen->QMax());

                ::Gen* g = new ::Gen(node, gen->id(), PMin, PMax, QMin, QMax);
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
            auto cBranch = branch.raw<const CommonBranch>();
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

            assert(std::abs(yShunt.real()) < 1e-9);
            arc->ch = yShunt.imag();

            arc->cc = arc->tr * cos(arc->as);
            arc->dd = arc->tr * sin(arc->as);

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
            auto sgtBus = sgtNw.buses()[node->_name];
            if (!sgtBus->isInService()) continue; // If not in service, ignore what PowerTools says.

            assert(sgtBus->gens().size() == node->_gen.size());

            Complex VSolPu(node->vr.get_value(), node->vi.get_value());
            Complex VSol = sgtNw.pu2V<Complex>(VSolPu, sgtBus->VBase());

            sgtBus->setV({VSol});

            Complex SLoadUnservedSolPu(-node->plv.get_value(), -node->qlv.get_value());
            Complex SLoadUnservedSol = sgtNw.pu2S(SLoadUnservedSolPu);
            if (std::abs(SLoadUnservedSol) > 1e-3)
            {
                sgtLogWarning() << "Unserved load at bus " << sgtBus->id() << " is " << SLoadUnservedSol << std::endl;
            }

            sgtBus->setSZipUnserved({SLoadUnservedSol});
            sgtBus->setSGenUnserved(arma::Col<Complex>(sgtBus->phases().size(), arma::fill::zeros));

            auto nGen = node->_gen.size();
            auto nSgtGen = sgtBus->gens().size();
            assert(nGen == nSgtGen);
            for (std::size_t i = 0; i < nGen; ++i)
            {
                // Order of gens should be same in Sgt and Pt.
                auto sgtGen = sgtBus->gens()[i].raw<Gen>();
                if (!sgtGen->isInService()) continue; // If not in service, ignore what PowerTools says.

                auto gen = node->_gen[i];
                Complex SGenSolPu(gen->pg.get_value(), gen->qg.get_value());
                Complex SGenSol = sgtNw.pu2S(SGenSolPu);
                sgtGen->setInServiceS({SGenSol});
            }
        }
    }

    void printNetw(const Net& net)
    {
        sgtLogMessage() << "Nodes-----------------" << std::endl;
        for (const auto node : net.nodes)
        {
            node->print();
        }
        sgtLogMessage() << "Arcs------------------" << std::endl;
        for (const auto arc : net.arcs)
        {
            arc->print();
        }
        sgtLogMessage() << "Gens------------------" << std::endl;
        for (const auto gen : net.gens)
        {
            gen->print();
        }
        sgtLogMessage() << "Done------------------" << std::endl;
    }

    void printModel(const Model& mod)
    {
        std::cout << "----vars----" << std::endl;
        for (auto v : mod._vars)
        {
            if (v->is_int())
            {
                static_cast<const var<int>*>(v)->print();
            }
            else if (v->is_binary())
            {
                static_cast<const var<bool>*>(v)->print();
            }
            else if (v->is_real())
            {
                static_cast<const var<float>*>(v)->print();
            }
            else if (v->is_longreal())
            {
                static_cast<const var<double>*>(v)->print();
            }
        }
        std::cout << "------------" << std::endl;
        std::cout << "----constrs----" << std::endl;
        for (auto cons : mod.get_cons())
        {
            cons->print();
        }
        std::cout << "------------" << std::endl;
        std::cout << "----objective----" << std::endl;
        mod._obj->print(false);
        std::cout << std::endl;
        std::cout << "------------" << std::endl;
    }
}
