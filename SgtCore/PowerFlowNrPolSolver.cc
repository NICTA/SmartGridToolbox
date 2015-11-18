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
#include "PowerFlowNrPolSolver.h"
#include "SparseHelper.h"
#ifdef WITH_KLU
#include "KluSolver.h"
#endif
#include "Stopwatch.h"

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
        SpMat<double> calcJ(
                const uword nPq,
                const uword nPv,
                const Col<double>& M,
                const Col<double>& theta,
                const SpMat<double>& G,
                const SpMat<double>& B)
        {
            uword nPqPv = nPq + nPv;
            arma::Col<double> c = cos(theta);
            arma::Col<double> s = sin(theta);

            SpMat<double> result;
            SparseHelper<double> helper(3 * nPqPv, 3 * nPqPv);
        
            auto cos_ik = [&](uword i, uword k) {return c(i) * c(k) + s(i) * s(k);};
            auto sin_ik = [&](uword i, uword k) {return s(i) * c(k) - c(i) * s(k);};
        
            auto iP = [&](uword i){return i - 1;};
            auto iQ = [&](uword i){return i - 1 + nPqPv;};
            auto iM = [&](uword i){return i - 1;};
            auto iT = [&](uword i){return i - 1 + nPq;};
        
            // -----------------------------------------------------------------
            // dP/dM_PQ
            
            // G terms in dP/dM_PQ
            {
                auto GSel = G.submat(1, 1, nPqPv, nPq);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iM(k), -M(i) * Gik * cos_ik(i, k));
                    }
                    else
                    {
                        helper.insert(iP(i), iM(i), -2 * M(i) * Gik);
                    }
                }
            }

            // B terms in dP/dM_PQ
            {
                auto BSel = B.submat(1, 1, nPqPv, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iM(k), M(i) * Bik * sin_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dP/dM_PQ
            {
                auto GSel = G.rows(1, nPqPv);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iM(i), -M(l) * Gil * cos_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dP/dM_PQ
            {
                auto BSel = B.rows(1, nPqPv);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Bil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iM(i), -M(l) * Bil * sin_ik(i, l));
                    }
                }
            }
            
            // -----------------------------------------------------------------
            // dP/dtheta

            // G terms in dP/dtheta
            {
                auto GSel = G.submat(1, 1, nPqPv, nPqPv);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1;
                    uword k = it.col() + 1;
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iT(k), -M(i) * M(k) * Gik * sin_ik(i, k));
                    }
                }
            }

            // B terms in dP/dtheta
            {
                auto BSel = B.submat(1, 1, nPqPv, nPqPv);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1;
                    uword k = it.col() + 1;
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iT(k), M(i) * M(k) * Bik * cos_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dP/dtheta
            {
                auto GSel = G.rows(1, nPqPv);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iT(i), M(i) * M(l) * Gil * sin_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dP/dtheta
            {
                auto BSel = B.rows(1, nPqPv);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Bil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iT(i), -M(i) * M(l) * Bil * cos_ik(i, l));
                    }
                }
            }
            
            // -----------------------------------------------------------------
            // dQ_PQ/dM_PQ

            // G terms in dQ_PQ/dM_PQ
            {
                auto GSel = G.submat(1, 1, nPq, nPq);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iM(k), -M(i) * Gik * sin_ik(i, k));
                    }
                }
            }
            
            // B terms in dQ_PQ/dM_PQ
            {
                auto BSel = B.submat(1, 1, nPq, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iM(k), M(i) * Bik * cos_ik(i, k));
                    }
                    else
                    {
                        helper.insert(iQ(i), iM(i), 2 * M(i) * Bik);
                    }
                }
            }

            // Inner sum G terms in dQ_PQ/dM_PQ
            {
                auto GSel = G.rows(1, nPq);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iM(i), -M(l) * Gil * sin_ik(i, l));
                    }
                }
            }
 
            // Inner sum B terms in dQ_PQ/dM_PQ
            {
                auto BSel = B.rows(1, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Bil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iM(i), M(l) * Bil * cos_ik(i, l));
                    }
                }
            }
            
            // -----------------------------------------------------------------
            // dQ_PQ/dtheta

            // G terms in dQ_PQ/dtheta
            {
                auto GSel = G.submat(1, 1, nPq, nPqPv);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iT(k), M(i) * M(k) * Gik * cos_ik(i, k));
                    }
                }
            }

            // B terms in dQ_PQ/dtheta
            {
                auto BSel = B.submat(1, 1, nPq, nPqPv);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword k = it.col() + 1; // Index in full set of busses.
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iT(k), M(i) * M(k) * Bik * sin_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dQ_PQ/dtheta
            {
                auto GSel = G.rows(1, nPq);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iT(i), -M(i) * M(l) * Gil * cos_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dQ_PQ/dtheta
            {
                auto BSel = B.rows(1, nPq);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row() + 1; // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    double Bil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iT(i), -M(i) * M(l) * Bil * sin_ik(i, l));
                    }
                }
            }

            // -----------------------------------------------------------------
              
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

    bool PowerFlowNrPolSolver::solveProblem()
    {
        sgtLogDebug() << "PowerFlowNrPolSolver : solve." << std::endl;
        LogIndent indent;

        mod_ = buildModel(*netw_);
        
        // Set up data structures for the calculation.
        // Model indexing is 0 = slack, [1 ... nPq] = PQ, [nPq + 1 ... nPq + nPv] = PV.

        uword nNode = mod_->nNode();
        uword nPq = mod_->nPq();
        uword nPv = mod_->nPv();
        uword nPqPv = nPq + nPv;

        const SpMat<Complex>& Y = mod_->Y(); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> G = real(Y); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = imag(Y); // Model indexing. Includes shunts (const Y in ZIPs).

        Col<Complex> V = mod_->V(); // Model indexing.
        Col<double> M = abs(V); // Model indexing.

        Col<double> theta(nNode, fill::none); // Model indexing.
        for (uword i = 0; i < nNode; ++i) theta(i) = std::arg(mod_->V()(i));

        Col<Complex> Scg = mod_->S(); // Model indexing. S_cg = S_c + S_g.

        const Col<Complex>& Ic = mod_->IConst(); // Model indexing. P_c + P_g.

        // Jacobian:
        SpMat<double> J = calcJ(nPq, nPv, M, theta, G, B);

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;
        bool ok;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogMessage(LogLevel::VERBOSE) << "iter " << niter << std::endl;
            LogIndent _;
            sgtLogDebug(LogLevel::VERBOSE) << "theta = " << theta << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "M = " << M << std::endl;

            auto S = calcS(Scg, Ic, V, M, Y);
            auto f = join_vert(real(S.subvec(1, nPq)), imag(S.subvec(1, nPqPv)));
            err = norm(f, "inf");
            sgtLogMessage(LogLevel::VERBOSE) << "Err = " << err << std::endl;
            if (err <= tol_) 
            {
                wasSuccessful = true;
                break;
            }

            Col<double> x; // Delta theta.
            ok = solveSparseSystem(J, f, x);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update M, theta, V.
            M.subvec(1, nPq) += x.subvec(0, nPq - 1);
            theta.subvec(1, nPqPv) += x.subvec(nPq, 2 * nPq + nPv - 1);
            Col<Complex> dir = cx_vec(cos(theta), sin(theta)); 
            V.subvec(1, nPqPv) = M.subvec(1, nPqPv) % dir.subvec(1, nPqPv);
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowFdSolver: failed to converge." << std::endl;
            // TODO.
        }

        return wasSuccessful;
    }
}
