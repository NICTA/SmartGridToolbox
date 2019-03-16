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

#ifndef POWER_FLOW_FD_DOT_H
#define POWER_FLOW_FD_DOT_H

#include "Common.h"
#include "PowerFlowModel.h"
#include "PowerFlowSolver.h"

// Terminology:
// "Bus" and "Branch" refer to n-phase objects i.e. they can contain several phases.
// "Node" and "Link" refer to individual bus conductors and single phase lines.
// A three phase network involving buses and branches can always be decomposed into a single phase network
// involving nodes and links. Thus use of buses and branches is simply a convenience that lumps together nodes and
// links.

namespace Sgt
{
    /// @brief Fast-decoupled NR power flow solver.
    /// @ingroup PowerFlowCore
    class PowerFlowFdSolver : public PowerFlowSolverInterface
    {
        public:

        virtual bool solve(Network& netw) override;

        public:

        double tol_{1e-8};
        unsigned int maxiter_{100};

        private:

        Network* netw_;
        std::unique_ptr<PowerFlowModel> mod_;

        arma::SpMat<double> G_;   ///< Real part of Y matrix.
        arma::SpMat<double> B_;   ///< Imag part of Y matrix.
    };
}

#endif // POWER_FLOW_FD_DOT_H
