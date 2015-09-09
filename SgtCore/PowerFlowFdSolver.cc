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
            return Scg + conj(Ic) % M - V % conj(Y * V);
        }

        // Build the Jacobian JP for P, theta. Indexing is [0 ... nPqPv - 1].
        SpMat<double> calcJP(
                const uword nPqPv,
                const Col<double>& M,
                const SpMat<double>& B)
        {
            SparseHelper<double> helper(nPqPv, nPqPv, true, false, false);

            for (uword i = 1; i <= nPqPv; ++i) // TODO: inefficient?
            {
                auto hadNz = false; 
                double sum = 0.0;
                for (auto it = B.begin_row(i); it != B.end_row(i); ++it)
                {
                    hadNz = true;
                    uword l = it.col();
                    if (l != i)
                    {
                        sum -= *it * M(l);
                    }
                }
                if (hadNz)
                {
                    helper.insert(i - 1, i - 1, sum * M(i));
                }
            }

            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
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
            
            SparseHelper<double> helper(nPq, nPq, true, false, false);

            for (uword i = 1; i <= nPq; ++i) // TODO: inefficient?
            {
                auto hadNz = false; 
                double sum = 0.0;
                for (auto it = B.begin_row(i); it != B.end_row(i); ++it)
                {
                    hadNz = true;
                    uword l = it.col();
                    if (l == i)
                    {
                        sum += 2 * M(i) * (*it);
                    }
                    else
                    {
                        sum += *it * M(l);
                    }
                }
                if (hadNz)
                {
                    helper.insert(i - 1, i - 1, sum);
                }
            }
           
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

    bool PowerFlowFdSolver::solveProblem()
    {
        sgtLogDebug() << "PowerFlowFdSolver : solve." << std::endl;
        LogIndent indent;

        mod_ = buildModel(*netw_);
        
        // Set up data structures for the calculation.
        // Model indexing is 0 = slack, [1 ... nPq] = PQ, [nPq + 1 ... nPq + nPv] = PV.

        uword nPq = mod_->nPq();
        uword nPv = mod_->nPv();
        uword nNode = mod_->nNode();
        uword nPqPv = nPq + nPv;

        const SpMat<Complex>& Y = mod_->Y(); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> G = real(Y); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = imag(Y); // Model indexing. Includes shunts (const Y in ZIPs).
        sgtLogDebug() << "B = " << B << std::endl;

        Col<Complex> V = mod_->V(); // Model indexing.
        Col<double> M = abs(V); // Model indexing.
        sgtLogDebug() << "M = " << M << std::endl;

        Col<double> theta(nNode, fill::none); // Model indexing.
        for (uword i = 0; i < nNode; ++i) theta(i) = std::arg(mod_->V()(i));
        sgtLogDebug() << "theta = " << theta << std::endl;

        Col<Complex> Scg = mod_->S(); // Model indexing. S_cg = S_c + S_g.

        const Col<Complex>& Ic = mod_->IConst(); // Model indexing. P_c + P_g.

        // Jacobian:
        SpMat<double> JP = calcJP(nPqPv, M, B);
        SpMat<double> JQ = calcJQ(nPq, M, B);
        sgtLogDebug() << "JP = " << JP << std::endl;
        sgtLogDebug() << "JQ = " << JQ << std::endl;

        bool wasSuccessful = false;
        double errP = 0;
        double errQ = 0;
        unsigned int niter;
        bool ok;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogDebug() << "theta = " << theta << std::endl;
            sgtLogDebug() << "M = " << M << std::endl;
            sgtLogMessage() << "iter " << niter << std::endl;
            LogIndent _;

            auto S = calcS(Scg, Ic, V, M, Y);
            Col<double> fP = real(S.subvec(1, nPqPv));
            Col<double> fQ = imag(S.subvec(1, nPq));
            sgtLogDebug() << "fP = " << fP << std::endl;
            sgtLogDebug() << "fQ = " << fQ << std::endl;

            errP = norm(fP, "inf");
            sgtLogMessage() << "Err_P = " << errP << std::endl;
            if (errP <= tol_)
            {
                wasSuccessful = true;
                break;
            }
            
            errQ = norm(fQ, "inf");
            sgtLogMessage() << "Err_Q = " << errQ << std::endl;
            if (errQ <= tol_)
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
            sgtLogDebug() << "xP = " << xP << std::endl;
            theta.subvec(1, nPqPv) += xP;
            Col<Complex> dir = cx_vec(cos(theta), sin(theta)); 
            V.subvec(1, nPqPv) = M.subvec(1, nPqPv) % dir.subvec(1, nPqPv);
            sgtLogDebug() << "theta new = " << theta << std::endl;
            sgtLogDebug() << "V new = " << V << std::endl;

            S = calcS(Scg, Ic, V, M, Y);
            fP = real(S.subvec(1, nPqPv));
            fQ = imag(S.subvec(1, nPq));
            sgtLogDebug() << "fQ = " << fQ << std::endl;

            Col<double> xQ; // Delta theta.
            ok = solveSparseSystem(JQ, fQ, xQ);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update M and V.
            sgtLogDebug() << "xQ = " << xQ << std::endl;
            M.subvec(1, nPq) += xQ;
            V.subvec(1, nPq) = M.subvec(1, nPq) % dir.subvec(1, nPq);
            sgtLogDebug() << "V new = " << V << std::endl;
            sgtLogDebug() << "M new = " << M << std::endl;
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowFdSolver: failed to converge." << std::endl;
            // TODO.
        }

        return wasSuccessful;
    }
}
