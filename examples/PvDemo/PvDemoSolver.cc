// Copyright 2015 National ICT Australia Limited (NICTA)
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

#include "PvDemoSolver.h"

#include "PvInverter.h"

#include <PowerTools++/Constraint.h>
#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>


namespace Sgt
{
    std::unique_ptr<PowerModel> PvDemoSolver::makeModel()
    {
        auto powerMod = std::unique_ptr<PowerModel>(new PowerModel(ACRECT, ptNetw_, ipopt));
        powerMod->build();
        powerMod->min_cost();
        auto mod = powerMod->_model;

        V2SlackA_.init("V2SlackA", 0, INFINITY);
        mod->addVar(V2SlackA_);
        
        V2SlackB_.init("V2SlackB", 0, INFINITY);
        mod->addVar(V2SlackB_);

        *mod->_obj += 10 * V2SlackA_;
        *mod->_obj += 2e3 * V2SlackB_;

        for (auto& cPair : mod->get_cons())
        {
            auto& c = *cPair;
            if (c._name == "V_LB")
            {
                c += V2SlackB_;
            }
            else if (c._name == "V_UB")
            {
                c -= V2SlackB_;
            }
        }

        for (auto n : powerMod->_net->nodes)
        {
            // Add constraints to try to achieve nominal voltage.
            {
                Constraint c(std::string("NOM_VOLTAGE_PLUS_") + std::to_string(n->ID));
                c += (n->_V_.square_magnitude());
                c -= V2SlackA_;
                c <= 1.01;
                mod->addConstraint(c);
            }
            {
                Constraint c(std::string("NOM_VOLTAGE_MINUS_") + std::to_string(n->ID));
                c += (n->_V_.square_magnitude());
                c += V2SlackA_;
                c >= 0.99;
                mod->addConstraint(c);
            }
        }
            
        for (auto bus : sgtNetw_->buses())
        {
            for (auto gen : bus->gens())
            {
                auto inverter = dynamic_cast<PvInverter*>(gen);
                if (inverter != nullptr)
                {
                    // Add an extra constraint for max apparent power.
                    auto ptBus = ptNetw_->nodeID.at(bus->id());
                    auto genId = inverter->id();
                    auto ptGen = std::find_if(ptBus->_gen.begin(), ptBus->_gen.end(), 
                            [genId](Gen* g){return g->_name == genId;});
                    assert(ptGen != ptBus->_gen.end());
                    Constraint c("PVD_SPECIAL_A");
                    c += ((**ptGen).pg^2) + ((**ptGen).qg^2);
                    double maxSMag = sgtNetw_->S2Pu(inverter->maxSMag());
                    c <= pow(maxSMag, 2);
                    mod->addConstraint(c);

                    // *mod->_obj += (**ptGen).qg^2; // Only use Q if we have to.
                }
            }
        }
        return powerMod;
    }
}
