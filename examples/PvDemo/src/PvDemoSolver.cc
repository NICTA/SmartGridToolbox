#include "PvDemoSolver.h"

#include "PvInverter.h"

#include <PowerTools++/Constraint.h>
#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>


namespace Sgt
{
    std::unique_ptr<PowerModel> PvDemoSolver::makeModel()
    {
        auto mod = PowerFlowPtPpSolver::makeModel();

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

                if (true)
                {
                    Constraint c("PVD_SPECIAL_A");
                    c += (((**gen).pg)^2) + (((**gen).qg)^2);
                    double maxSMag = sgtNetw_->S2Pu(inverter->maxSMag());
                    c <= pow(maxSMag, 2);
                    // c.print(); std::cout << std::endl;
                    mod->_model->addConstraint(c);
                }

                if (false)
                {
                    Constraint c("PVD_SPECIAL_B");
                    c += (((**gen).pg)^2) - (((**gen).qg)^2);
                    c >= 0;
                    // c.print(); std::cout << std::endl;
                    mod->_model->addConstraint(c);
                }
            }
        }
        // mod->_model->print();
        return mod;
    }
}
