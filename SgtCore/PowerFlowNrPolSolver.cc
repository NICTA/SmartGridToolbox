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

#include "config.h"
#include "Network.h"
#include "PowerFlowNrPolSolver.h"
#include "SparseHelper.h"
#ifdef WITH_KLU
#include "KluSolver.h"
#endif

#include <algorithm>
#include <ostream>
#include <sstream>

using namespace Sgt;
using namespace arma;

namespace Sgt
{
    namespace
    {
        Col<Complex> calcS(
                const Col<Complex>& Scg,
                const Col<Complex>& Ic,
                const Col<Complex>& V,
                const Col<double>& M,
                const SpMat<Complex>& Y)
        {
            return (Scg + conj(Ic) % M - V % conj(Y * V));
        }

        void updateQcgPv(
                Col<Complex>& Scg,
                span selPv,
                const Col<Complex>& Ic,
                const Col<Complex>& V,
                const Col<double>& M,
                const SpMat<Complex>& Y)
        {
            // TODO: may unnessecarily calculate P.
            Col<double> PSv = real(Scg(selPv));
            Col<double> QNew = imag(Ic(selPv)) % M(selPv) + imag(V(selPv) % conj(Y.rows(selPv.a, selPv.b) * V)); 
            Scg(selPv) = cx_mat(PSv, QNew); 
        }

        void updateScgSl(
                Col<Complex>& Scg,
                span selSl,
                const Col<Complex>& Ic,
                const Col<Complex>& V,
                const Col<double>& M,
                const SpMat<Complex>& Y)
        {
            Scg(selSl) = -conj(Ic(selSl)) % M(selSl) + V(selSl) % conj(Y.rows(selSl.a, selSl.b) * V);
        }

        template<typename T> SpMat<typename T::elem_type> spDiag(const T& v)
        {
            uword n = v.size();
            Mat<uword> locs(2, n, fill::none);
            locs.row(0) = linspace<Row<uword>>(0, n - 1, n);
            locs.row(1) = locs.row(0);
            return SpMat<typename T::elem_type>(locs, v, false);
        }

        // Used for the Jacobian. See Matpower's dSbus_dV.
        // Note however that we're using a negative definition for S compared to matpower.
        // First returned element is dS_dVm, second is dS_dVa. 
        std::pair<SpMat<Complex>, SpMat<Complex>> dSdV(const SpMat<Complex>& Y, const Col<Complex>& V)
        {
            Col<Complex> I = Y * V;
            SpMat<Complex> diagI = spDiag(I);
            SpMat<Complex> diagV = spDiag(V);
            Col<Complex> VNorm = V / abs(V);
            SpMat<Complex> diagVNorm = spDiag(VNorm);
            return {
                -(diagV * conj(Y * diagVNorm) + conj(diagI) * diagVNorm),
                -Complex{0, 1} * diagV * conj(diagI - Y * diagV)
            };
        }
        
        SpMat<double> calcJ(
                const uword nPq,
                const uword nPv,
                const Col<Complex>& V,
                const SpMat<Complex>& Y)
        {
            uword nPqPv = nPq + nPv;

            auto iPq = [&](uword i){return i;};
            auto iPV = [&](uword i){return i + nPq;};

            auto allPq = span(iPq(0), iPq(nPq - 1));
            auto allPqPV = span(iPq(0), iPV(nPv - 1));

            uword szJ = 2 * nPq + nPv;

            auto dSdV_ = dSdV(Y, V);
            const auto& dSdM = dSdV_.first;
            const auto& dSdT = dSdV_.second;

            SparseHelper<double> h(szJ, szJ);
            if (nPq > 0)
            {
                SpMat<double> dPdM = real(dSdM.submat(allPqPV, allPq));
                for (auto it = dPdM.begin(); it != dPdM.end(); ++it)
                {
                    uword i = it.row(); uword k = it.col(); double val = *it;
                    h.insert(i, k, val);
                }
                
                SpMat<double> dQdM = imag(dSdM.submat(allPq, allPq));
                for (auto it = dQdM.begin(); it != dQdM.end(); ++it)
                {
                    uword i = it.row(); uword k = it.col(); double val = *it;
                    h.insert(i + nPqPv, k, val);
                }

                SpMat<double> dQdT = imag(dSdT.submat(allPq, allPqPV));
                for (auto it = dQdT.begin(); it != dQdT.end(); ++it)
                {
                    uword i = it.row(); uword k = it.col(); double val = *it;
                    h.insert(i + nPqPv, k + nPq, val);
                }
            }

            SpMat<double> dPdT = real(dSdT.submat(allPqPV, allPqPV));
            for (auto it = dPdT.begin(); it != dPdT.end(); ++it)
            {
                uword i = it.row(); uword k = it.col(); double val = *it;
                h.insert(i, k + nPq, val);
            }

            return h.get();
        }

        // Solve Jx + f = 0
        bool solveSparseSystem(const SpMat<double>& J, const Col<double>& f, Col<double>& x)
        {
            bool ok;
#ifdef WITH_KLU
            ok = kluSolve(J, -f, x);
#else
            ok = spsolve(x, J, -f, "superlu");
#endif
            return ok;
        }
    }

    bool PowerFlowNrPolSolver::solve(Network& netw)
    {
        sgtLogDebug() << "PowerFlowNrPolSolver : solve for " << netw.islands().size() << " islands." << std::endl;
        sgtLogIndent();

        netw_ = &netw;
        bool ok = true;
        for (auto& island : netw.islands())
        {
            sgtLogDebug() << "island " << island.idx << " isSupplied = " << island.isSupplied
                << " n_buses = " << island.buses.size() << std::endl;
            if (island.isSupplied)
            {
                ok = solveForIsland(island.idx) && ok;
            }
        }
        return ok;
    }

    bool PowerFlowNrPolSolver::solveForIsland(int islandIdx)
    {
        sgtLogDebug() << "PowerFlowNrPolSolver : solving for island " << islandIdx << std::endl;
        sgtLogIndent();
        bool ok = true;
        mod_ = buildModel(*netw_, [islandIdx](const Bus& b){return b.islandIdx() == islandIdx;});

        // Cache V, Scg, IConst, as these are calculated and not cached in the model.
        Col<Complex> V = mod_->V(); // Model indexing.
        Col<Complex> Scg = mod_->Scg(); // Model indexing. S_cg = S_c + S_g.
        const Col<Complex>& Ic = mod_->IConst(); // Model indexing. P_c + P_g.
        
        // Set up data structures for the calculation.
        // Model indexing is [0 ... nPq - 1] = Pq, [nPq ... nPq + nPv - 1] = PV, [nPq + nPv ... nPq + nPv + nSl] = SL

        uword nPqPv = mod_->nPq() + mod_->nPv();

        const SpMat<Complex>& Y = mod_->Y(); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> G = real(Y); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = imag(Y); // Model indexing. Includes shunts (const Y in ZIPs).

        Col<double> M = abs(V); // Model indexing.

        Col<double> theta(mod_->nNode(), fill::none); // Model indexing.
        for (uword i = 0; i < mod_->nNode(); ++i) theta(i) = std::arg(V(i));

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogMessage(LogLevel::VERBOSE) << "iter " << niter << std::endl;
            sgtLogIndent();

            auto S = calcS(Scg, Ic, V, M, Y);
            Col<double> f = mod_->nPq() > 0
                ? join_vert(real(S.subvec(0, nPqPv - 1)), imag(S.subvec(0, mod_->nPq() - 1))).eval()
                : real(S.subvec(0, nPqPv - 1));
            err = norm(f, "inf");
            sgtLogMessage(LogLevel::VERBOSE) << "Err = " << err << std::endl;
            if (err <= tol_) 
            {
                wasSuccessful = true;
                break;
            }

            SpMat<double> J = calcJ(mod_->nPq(), mod_->nPv(), V, Y);

            Col<double> x; // Delta theta.
            ok = solveSparseSystem(J, f, x);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }
            sgtLogDebug(LogLevel::VERBOSE) << "--------------------" << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "V = " << V << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "S = " << S << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "f = " << f << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "|f| = " << err << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "x = " << x << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "J =\n" << std::setw(10) << Mat<double>(J) << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "--------------------" << std::endl;

            // Update M, theta, V.
            if (mod_->nPq() > 0)
            {
                M.subvec(0, mod_->nPq() - 1) += x.subvec(0, mod_->nPq() - 1);
            }
            theta.subvec(0, nPqPv - 1) += x.subvec(mod_->nPq(), 2 * mod_->nPq() + mod_->nPv() - 1);
            V.subvec(0, nPqPv - 1) = M.subvec(0, nPqPv - 1) % exp(Complex{0, 1} * theta.subvec(0, nPqPv - 1));
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowNrPolSolver: failed to converge." << std::endl;
        }

        if (mod_->nPv() > 0)
        {
            updateQcgPv(Scg, mod_->selPv(), Ic, V, M, Y);
        }
        if (mod_->nSl() > 0)
        {
            updateScgSl(Scg, mod_->selSl(), Ic, V, M, Y);
        }

        mod_->setV(V);
        mod_->setScg(Scg);

        if (wasSuccessful)
        {
            applyModel(*mod_, *netw_);
        }
        
        sgtLogDebug() << "Finished: successful = " << wasSuccessful << std::endl;

        return wasSuccessful;
    }
}
