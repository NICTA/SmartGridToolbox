#include "PvDemo.h"

#include <SgtCore/Network.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/SimNetwork.h>

#include <PowerTools++/Constraint.h>
#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>


namespace Sgt
{
    void PvInverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto inverter = sim.newSimComponent<PvInverter>(id, busId);

        if (nd["efficiency"])
        {
            inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }

        const std::string networkId = parser.expand<std::string>(nd["network_id"]);
        auto network = sim.simComponent<SimNetwork>(networkId);
        network->addGen(inverter, busId);
    }

    std::unique_ptr<PowerModel> PvDemoSolver::makeModel()
    {
        auto mod = PowerFlowPtPpSolver::makeModel();
        VBoundViol_ = var<double>("V_bound_viol", 0.0, INFINITY);
        mod->_model->addVar(VBoundViol_);

        for (auto nd : ptNetw_->nodes)
        {
            {
                Constraint c(std::string("V_ub_") + nd->_name);
                c += (nd->vr^2) + (nd->vi^2) - VBoundViol_;
                c <= pow(nd->vbound.max,2);                                                                  
                mod->_model->addConstraint(c);                                                                    
            }
           
            {
                Constraint c(std::string("V_lb_") + nd->_name);
                c += (nd->vr^2) + (nd->vi^2) + VBoundViol_;                                                       
                c >= pow(nd->vbound.min,2);                                                                 
                mod->_model->addConstraint(c);                                                                    
            }
        }

        *mod->objective() += 10000 * VBoundViol_;

        for (auto gen : sgtNetw_->gens())
        {
            auto inverter = std::dynamic_pointer_cast<PvInverter>(gen);
            if (inverter != nullptr)
            {
                // Add an extra constraint for max apparent power.
                auto bus = ptNetw_->nodeID.at(inverter->busId_);
                auto genId = inverter->id();
                auto gen = std::find_if(bus->_gen.begin(), bus->_gen.end(), 
                        [genId](Gen* gen){return gen->_name == genId;});
                assert(gen != bus->_gen.end());

                {
                    Constraint c("PVD_SPECIAL_A");
                    c += ((**gen).pg)^2;
                    c += ((**gen).qg)^2;
                    double maxSMag = sgtNetw_->S2Pu(inverter->maxSMag());
                    c <= pow(maxSMag, 2);
                    // c.print(); std::cout << std::endl;
                    mod->_model->addConstraint(c);
                }

                {
                    Constraint c("PVD_SPECIAL_B");
                    c += ((**gen).pg)^2;
                    c -= ((**gen).qg)^2;
                    c >= 0;
                    // c->print(); std::cout << std::endl;
                    mod->_model->addConstraint(c);
                }
            }
        }
        // mod->_model->print();
        return mod;
    }
}
