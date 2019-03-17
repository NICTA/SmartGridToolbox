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

#include "PowerFlowPtSolver.h"

#include "Network.h"
#include "PowerToolsSupport.h"
#include "CommonBranch.h"
#include "Stopwatch.h"

#include <PowerTools++/PowerModel.h>

#include <set>

namespace Sgt
{
    PowerFlowPtSolver::PowerFlowPtSolver() {};
    PowerFlowPtSolver::~PowerFlowPtSolver() {};

    bool PowerFlowPtSolver::solve(Network& netw)
    {
        sgtNetw_ = &netw;
        ptNetw_ = sgt2PowerTools(*sgtNetw_);
        // printNetw(*ptNetw_);
        
        Stopwatch stopwatchSolve;
        stopwatchSolve.start();
        auto pModel = makeModel();
        int retVal = pModel->solve();
        bool success = (retVal != -1);
        stopwatchSolve.stop();
        
        // printNetw(*ptNetw_);
        sgtLogMessage() << "PowerFlowPtSolver:" << std::endl;
        sgtLogIndent();
        sgtLogMessage() << "Solve time          = " << stopwatchSolve.cpuSeconds() << std::endl;
        if (success)
        {
            powerTools2Sgt(*ptNetw_, *sgtNetw_);
        }
        return success;
    }

    std::unique_ptr<PowerModel> PowerFlowPtSolver::makeModel()
    {
        std::unique_ptr<PowerModel> mod(new PowerModel(ACRECT, ptNetw_.get(), ipopt)); // PowerModel doesn't own net.
        mod->build();
        mod->min_cost();
        return mod;
    }
}
