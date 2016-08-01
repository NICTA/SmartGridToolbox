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

#ifndef POWER_FLOW_NR_RECT_SOLVER_DOT_H
#define POWER_FLOW_NR_RECT_SOLVER_DOT_H

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
    struct Jacobian
    {
        arma::SpMat<Complex> dDdVr;
        arma::SpMat<Complex> dDdVi;
        arma::SpMat<Complex> dDPvdQPv;

        Jacobian(arma::uword nPq, arma::uword nPv);
    };

    /// @brief Newton-Raphson AC power flow solver.
    /// @ingroup PowerFlowCore
    class PowerFlowNrRectSolver : public PowerFlowSolverInterface
    {
        public:

            virtual bool solve(Network& netw) override;

        private:
            
            void init(int islandIdx);
    
            bool solveForIsland(int islandIdx);

            arma::uword nVar() const
            {
                return 2 * mod_->nPqPv();
            }

            void initSubmatrixRanges();

            arma::Col<Complex> calcD(
                    const arma::Col<Complex>& V, 
                    const arma::Col<Complex>& Scg,
                    const arma::Col<double>& M2PvSetpt) const;

            Jacobian calcJ(
                    const arma::Col<Complex>& V,
                    const arma::Col<Complex>& Scg,
                    const arma::Col<double>& M2PvSetpt) const;

            void modifyForPv(
                    Jacobian& J,
                    arma::Col<Complex>& D,
                    const arma::Col<double>& Vr,
                    const arma::Col<double>& Vi,
                    const arma::Col<double>& M2,
                    const arma::Col<double>& M2PvSetpt);

            arma::Col<double> construct_f(const arma::Col<Complex>&D) const;

            arma::SpMat<double> constructJMatrix(const Jacobian& J) const;
        
            arma::Col<Complex> calcSGenSl(const arma::Col<Complex>& V, const arma::Col<double>& M);

        public:

            double tol_{1e-8};
            unsigned int maxiter_{100};

        private:

            Network* netw_;
            std::unique_ptr<PowerFlowModel> mod_;

            arma::Col<arma::uword> selDrFrom_f_;
            arma::Col<arma::uword> selDiFrom_f_;

            arma::Col<arma::uword> selVrOrQFrom_x_;
            arma::Col<arma::uword> selViFrom_x_;
    };
}

#endif // POWER_FLOW_NR_RECT_SOLVER_DOT_H
