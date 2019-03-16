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
#include "PowerFlowFdSolver.h"
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
            return (Scg + conj(Ic) % M - V % conj(Y * V)) / M;
        }

        // Build the Jacobian JP for P, theta. Indexing is [0 ... nPqPv - 1].
        SpMat<double> calcJP(
                const uword nPqPv,
                const Col<double>& M,
                const SpMat<Complex>& Y)
        {
            // Note that the diagonal terms of B do not make an appearance in
            // the following. This means that it is easy to zero out R.
            SpMat<double> B1;
            {
                SparseHelper<double> helper(Y.n_rows, Y.n_rows, false, false, false);
                for (auto it = Y.begin(); it != Y.end(); ++it)
                {
                    auto i = it.row();
                    auto k = it.col();
                    if (i != k)
                    {
                        auto val = *it;
                        helper.insert(i, k, -norm(val) / imag(val)); // Remember, norm in std::complex is |x|^2.
                    }
                }
                B1 = helper.get();
            }

            SparseHelper<double> helper(nPqPv, nPqPv, true, true, true);

            auto BSel = B1.rows(1, nPqPv);
            for (auto it = BSel.begin(); it != BSel.end(); ++it)
            {
                uword i = it.row() + 1; // Convert to index in full set of buses.
                uword l = it.col();
                double Bil = *it;
                if (i != l)
                {
                    helper.insert(i - 1, i - 1, -Bil * M(l));
                }
            }

            for (SpMat<double>::iterator it = B1.begin(); it != B1.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                if (i > 0 && k > 0 && i != k)
                {
                    helper.insert(i - 1, k - 1, *it * M(i) * M(k));
                }
            }

            return helper.get();
        }

        SpMat<double> calcJQ(
                const uword nPq,
                const Col<double>& M,
                const SpMat<double>& B)
        {
            // Note: shunt terms have already been absorbed into Y (PowerFlowModel).
            
            SparseHelper<double> helper(nPq, nPq, true, true, true);

            {
                auto BSel = B.rows(1, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Convert to index in full set of buses.
                    uword l = it.col();
                    double Bil = *it;
                    if (i == l)
                    {
                        helper.insert(i - 1, i - 1, 2 * M(i) * Bil);
                    }
                    else
                    {
                        helper.insert(i - 1, i - 1, Bil * M(l));
                    }
                }
            }

            {
                auto BSel = B.submat(1, 1, nPq, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row();
                    uword k = it.col();

                    if (i != k)
                    {
                        helper.insert(i, k, *it * M(i));
                    }
                }
            }

            return helper.get();
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

    bool PowerFlowFdSolver::solve(Network& netw)
    {
        sgtLogDebug() << "PowerFlowFdSolver : solve." << std::endl;
        sgtLogIndent();

        netw_ = &netw;
        mod_ = buildModel(*netw_);

        // Cache V, Scg, IConst, as these are calculated and not cached in the model.
        Col<Complex> V = mod_->V(); // Model indexing.
        Col<Complex> Scg = mod_->Scg(); // Model indexing. S_cg = S_c + S_g.
        const Col<Complex>& Ic = mod_->IConst(); // Model indexing. P_c + P_g.
        
        // Set up data structures for the calculation.
        // Model indexing is 0 = slack, [1 ... nPq] = PQ, [nPq + 1 ... nPq + nPv] = PV.

        uword nPq = mod_->nPq();
        uword nPv = mod_->nPv();
        uword nNode = mod_->nNode();
        uword nPqPv = nPq + nPv;

        const SpMat<Complex>& Y = mod_->Y(); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> G = real(Y); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = imag(Y); // Model indexing. Includes shunts (const Y in ZIPs).

        Col<double> M = abs(V); // Model indexing.

        Col<double> theta(nNode, fill::none); // Model indexing.
        for (uword i = 0; i < nNode; ++i) theta(i) = std::arg(V(i));

        // Jacobian:
        SpMat<double> JP = calcJP(nPqPv, M, Y);
        SpMat<double> JQ = calcJQ(nPq, M, B);

        bool wasSuccessful = false;
        double errP = 0;
        double errQ = 0;
        unsigned int niter;
        bool ok;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogMessage() << "iter " << niter << std::endl;
            sgtLogIndent();
            sgtLogDebug() << "theta = " << theta << std::endl;
            sgtLogDebug() << "M = " << M << std::endl;

            auto S = calcS(Scg, Ic, V, M, Y);
            Col<double> fP = real(S.subvec(1, nPqPv));
            Col<double> fQ = imag(S.subvec(1, nPq));
            errP = norm(fP, "inf");
            errQ = norm(fQ, "inf");
            sgtLogMessage() << "Err_P = " << errP << std::endl;
            sgtLogMessage() << "Err_Q = " << errQ << std::endl;
            if (errP <= tol_ && errQ <= tol_)
            {
                wasSuccessful = true;
                break;
            }

            Col<double> xP; // Delta theta.
            ok = solveSparseSystem(JP, fP, xP);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update theta.
            theta.subvec(1, nPqPv) += xP;
            sgtLogDebug() << "-> theta = " << theta << std::endl;
            Col<Complex> dir = cx_vec(cos(theta), sin(theta)); 
            V.subvec(1, nPqPv) = M.subvec(1, nPqPv) % dir.subvec(1, nPqPv);

            S = calcS(Scg, Ic, V, M, Y);
            fP = real(S.subvec(1, nPqPv));
            fQ = imag(S.subvec(1, nPq));

            Col<double> xQ; // Delta M.
            ok = solveSparseSystem(JQ, fQ, xQ);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update M and V.
            M.subvec(1, nPq) += xQ;
            sgtLogDebug() << "-> M = " << M << std::endl;
            V.subvec(1, nPq) = M.subvec(1, nPq) % dir.subvec(1, nPq);
        }

        if (wasSuccessful)
        {
            applyModel(*mod_, *netw_);
        }
        else
        {
            sgtLogWarning() << "PowerFlowFdSolver: failed to converge." << std::endl;
            // TODO.
        }

        return wasSuccessful;
    }
}
