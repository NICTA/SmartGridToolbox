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

namespace
{
    template<typename T> Col<Complex> colConj(const T& from)
    {
        Col<Complex> result(from.n_elem, fill::none);
        for (uword i = 0; i < from.n_elem; ++i)
        {
            result(i) = std::conj(from(i));
        }
        return result;
    }

    template<typename T, typename U> void initJcBlock(const T& G, const T& B, U& Jrr, U& Jri, U& Jir, U& Jii)
    {
        Jrr = -G;
        Jri =  B;
        Jir = -B;
        Jii = -G;
    }
}

namespace Sgt
{
    namespace
    {

        // Build the Jacobian JP for P, theta. Indexing is [0 ... nPqPv - 1].
        SpMat<double> calcJP(
                const uword nPqPv,
                const Col<double>& M,
                const SpMat<double>& B)
        {
            sgtLogDebug() << "Building JP..." << std::endl;
            LogIndent _;
            SparseHelper<double> helper(nPqPv, nPqPv, false, false, false);

            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                if (i > 0 && k > 0)
                {
                    if (i != k)
                    {
                        helper.insert(i - 1, k - 1, *it * M(i) * M(k));
                    }
                    else
                    {
                        double sum = 0;
                        auto row = B.row(i);
                        for (auto rowIt = B.begin_row(i); rowIt != B.end_row(i); ++rowIt)
                        {
                            if (rowIt.col() != k)
                            {
                                sum += *rowIt;
                            }
                        }
                        helper.insert(i - 1, i - 1, -sum * M(i) * M(i));
                    }
                }
            }

            sgtLogDebug() << "Finished." << std::endl;
            return helper.get();
        }

        SpMat<double> calcJQ(
                const uword nPqPv, const uword nPq, const uword nPv,
                const Col<double>& M,
                const SpMat<double>& B)
        {
            sgtLogDebug() << "Building JQ..." << std::endl;
            LogIndent _;
            
            // Note: shunt terms have already been absorbed into Y (PowerFlowModel).
            
            SparseHelper<double> helper(nPqPv, nPqPv, true, false, false);
            
            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                
                if (i > 0 && k > 0 && k <= nPq)
                {
                    double val = *it * M(i); if (i == k) val *= 2;
                    helper.insert(i - 1, k - 1, val);
                }
            }

            for (uword i = nPq; i < nPqPv; ++i)
            {
                helper.insert(i, i, 1.0);
            }

            sgtLogDebug() << "Finished." << std::endl;
            return helper.get();
        }

        Col<double> calc_fP(
                const uword nPqPv,
                const Col<double>& Pcg, const Col<double>& Irc,
                const Col<double>& M, const Col<double>& theta,
                const SpMat<double>& G, const SpMat<double>& B)
        {
            // TODO: efficiency.

            Col<double> cosTheta = cos(theta);
            Col<double> sinTheta = sin(theta);

            Col<double> P = Pcg + Irc % M;

            for (SpMat<double>::iterator it = G.begin(); it != G.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                P(i) -= *it * (cosTheta(i) * cosTheta(k) + sinTheta(i) * sinTheta(k)) * M(i) * M(k);
            }

            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                P(i) -= *it * (sinTheta(i) * cosTheta(k) - cosTheta(i) * sinTheta(k)) * M(i) * M(k);
            }

            sgtLogDebug() << "P = " << P << std::endl;

            return P.subvec(1, nPqPv);
        }

        Col<double> calc_fQ(
                const uword nPqPv,
                const Col<double>& Qcg, const Col<double>& Iic,
                const Col<double>& M, const Col<double>& theta,
                const SpMat<double>& G, const SpMat<double>& B)
        {
            // TODO: efficiency.
            
            Col<double> cosTheta = cos(theta);
            Col<double> sinTheta = sin(theta);

            Col<double> Q = Qcg - Iic % M;

            for (SpMat<double>::iterator it = G.begin(); it != G.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                Q(i) -= *it * (sinTheta(i) * cosTheta(k) - cosTheta(i) * sinTheta(k)) * M(i) * M(k);
            }

            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                Q(i) += *it * (cosTheta(i) * cosTheta(k) + sinTheta(i) * sinTheta(k)) * M(i) * M(k);
            }

            sgtLogDebug() << "Q = " << Q << std::endl;

            return Q.subvec(1, nPqPv);
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
        sgtLogDebug() << "YBus = " << mod_->Y() << std::endl;
        
        // Set up data structures for the calculation.
        // Model indexing is 0 = slack, [1 ... nPq] = PQ, [nPq + 1 ... nPq + nPv] = PV.

        uword nPq = mod_->nPq();
        sgtLogDebug() << "nPq = " << nPq << std::endl;
        uword nPv = mod_->nPv();
        sgtLogDebug() << "nPv = " << nPv << std::endl;
        uword nNode = mod_->nNode();
        sgtLogDebug() << "nNode = " << nNode << std::endl;
        uword nPqPv = nPq + nPv;

        SpMat<double> G = real(mod_->Y()); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = imag(mod_->Y()); // Model indexing. Includes shunts (const Y in ZIPs).

        Col<double> M = abs(mod_->V()); // Model indexing.
        sgtLogDebug() << "M = " << M << std::endl;

        Col<double> theta(nNode, fill::none); // Model indexing.
        for (uword i = 0; i < nNode; ++i) theta(i) = std::arg(mod_->V()(i));
        sgtLogDebug() << "theta = " << theta << std::endl;

        Col<double> Pcg = real(mod_->S()); // Model indexing. Pcg = P_c + P_g.
        Col<double> Qcg = imag(mod_->S()); // Model indexing. Qcg = Q_c + Q_g.

        Col<double> Irc = real(mod_->IConst()); // Model indexing. P_c + P_g.
        Col<double> Iic = imag(mod_->IConst()); // Model indexing. P_c + P_g.

        // Jacobian:
        SpMat<double> JP = calcJP(nPqPv, M, B);
        SpMat<double> JQ = calcJQ(nPqPv, nPq, nPv, M, B);

        bool wasSuccessful = false;
        double errP = 0;
        double errQ = 0;
        unsigned int niter;
        bool ok;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogDebug() << "iter " << niter << std::endl;

            // P subsystem:
            
            Col<double> fP = calc_fP(nPqPv, Pcg, Irc, M, theta, G, B);
            sgtLogDebug() << "fP = " << fP << std::endl;
            sgtLogDebug() << "JP = " << JP << std::endl;
Col<double> fQDebug = calc_fQ(nPqPv, Qcg, Iic, M, theta, G, B);
            sgtLogDebug() << "fQ = " << fQDebug << std::endl;

            errP = norm(fP, "inf");
            sgtLogDebug() << "Err_P = " << errP << std::endl;
            if (errP <= tol_)
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

            // Q subsystem:
            
            Col<double> fQ = calc_fQ(nPqPv, Qcg, Iic, M, theta, G, B);
            sgtLogDebug() << "fQ = " << fQ << std::endl;

            errQ = norm(fQ, "inf");
            sgtLogDebug() << "Err_Q = " << errQ << std::endl;
            if (errQ <= tol_)
            {
                wasSuccessful = true;
                break;
            }

            Col<double> xQ; // Delta theta.
            ok = solveSparseSystem(JQ, fQ, xQ);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update M and Qcg.
            if (nPq > 0) M.subvec(1, nPq) += xQ.subvec(0, nPq - 1);
            if (nPv > 0) Qcg.subvec(nPq + 1, nPqPv) += xQ.subvec(nPq, nPqPv - 1);
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowFdSolver: failed to converge." << std::endl;
            // TODO.
        }

        return wasSuccessful;
    }
}
