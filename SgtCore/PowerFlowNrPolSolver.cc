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
            return (Scg + conj(Ic) % M - V % conj(Y * V));
        }

        template<typename T, typename U> auto dM(const T& d, const U& M) -> decltype(diag(d) * M)
        {
            using Result = decltype(dM(std::declval<const T&>(), std::declval<const U&>()));
            Result result = M;
            for (auto it = result.begin(); it != result.end(); ++it)
            {
                *it *= d(it.row());
            }
            return result;
        }

        template<typename T, typename U> auto Md(const T& M, const U& d) -> decltype(M * diag(d))
        {
            using Result = decltype(Md(std::declval<const T&>(), std::declval<const U&>()));
            Result result = M;
            for (auto it = result.begin(); it != result.end(); ++it)
            {
                *it *= d(it.row());
            }
            return result;
        }

        template<typename T, typename U> auto addDiag(const T& M, const U& d) -> decltype(M + diag(d))
        {
            using Result = decltype(addDiag(std::declval<const T&>(), std::declval<const U&>()));
            Result result = M;
            for (auto it = result.begin(); it != result.end(); ++it)
            {
                *it += d(it.row());
            }
            return result;
        }

        template<typename T> SpMat<typename T::elem_type> spDiag(const T& v)
        {
            uword n = v.size();
            Mat<uword> locs(2, n, arma::fill::none);
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
        
        SpMat<double> calcJ1(
                const uword nPq,
                const uword nPv,
                const Col<Complex>& V,
                const SpMat<Complex>& Y)
        {
            uword nPqPv = nPq + nPv;

            auto iP = [&](uword i){return i;};
            auto iQ = [&](uword i){return i + nPqPv;};
            auto iM = [&](uword i){return i;};
            auto iT = [&](uword i){return i + nPq;};
            auto iPQ = [&](uword i){return i;};
            auto iPV = [&](uword i){return i + nPq;};

            auto allP = span(iP(0), iP(nPqPv - 1));
            auto allQ = span(iQ(0), iQ(nPq - 1));
            auto allM = span(iM(0), iM(nPq - 1));
            auto allT = span(iT(0), iT(nPqPv - 1));
            auto allPQ = span(iPQ(0), iPQ(nPq - 1));
            auto allPQPV = span(iPQ(0), iPV(nPv - 1));

            uword szJ = 2 * nPq + nPv;

            Stopwatch sw;
            sw.start();
            auto dSdV_ = dSdV(Y, V);
            sw.stop();
            std::cout << "time in dSdV = " << sw.seconds() << std::endl;
            sw.reset();
            const auto& dSdM = dSdV_.first;
            const auto& dSdT = dSdV_.second;

            SpMat<double> dPdM = real(dSdM.submat(allPQPV, allPQ));
            SpMat<double> dQdM = imag(dSdM.submat(allPQ, allPQ));
            SpMat<double> dPdT = real(dSdT.submat(allPQPV, allPQPV));
            SpMat<double> dQdT = imag(dSdT.submat(allPQ, allPQPV));
            
            SparseHelper<double> h(szJ, szJ);
            for (auto it = dPdM.begin(); it != dPdM.end(); ++it)
            {
                uword i = it.row(); uword k = it.col(); double val = *it;
                h.insert(i, k, val);
            }
            for (auto it = dQdM.begin(); it != dQdM.end(); ++it)
            {
                uword i = it.row(); uword k = it.col(); double val = *it;
                h.insert(i + nPqPv, k, val);
            }
            for (auto it = dPdT.begin(); it != dPdT.end(); ++it)
            {
                uword i = it.row(); uword k = it.col(); double val = *it;
                h.insert(i, k + nPq, val);
            }
            for (auto it = dQdT.begin(); it != dQdT.end(); ++it)
            {
                uword i = it.row(); uword k = it.col(); double val = *it;
                h.insert(i + nPqPv, k + nPq, val);
            }

            return h.get();
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

            SparseHelper<double> helper(2 * nPq + nPv, 2 * nPq + nPv);
        
            auto cos_ik = [&](uword i, uword k) {return c(i) * c(k) + s(i) * s(k);};
            auto sin_ik = [&](uword i, uword k) {return s(i) * c(k) - c(i) * s(k);};
        
            auto iP = [&](uword i){return i;};
            auto iQ = [&](uword i){return i + nPqPv;};
            auto iM = [&](uword i){return i;};
            auto iT = [&](uword i){return i + nPq;};
        
            // -----------------------------------------------------------------
            // dP/dM_PQ
            
            // G terms in dP/dM_PQ
            {
                auto GSel = G.submat(0, 0, nPqPv - 1, nPq - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iM(k) == 0) std::cout << "A " << i << " " << k << std::endl;
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iM(k), -M(i) * Gik * cos_ik(i, k));
                        if (iP(i) == 2 && iM(k) == 0) std::cout << "    A " << -M(i) * Gik * cos_ik(i, k) << " " << cos_ik(i, k) << std::endl;
                    }
                    else
                    {
                        helper.insert(iP(i), iM(k), -2 * M(i) * Gik);
                        if (iP(i) == 2 && iM(k) == 0) std::cout << "    B " << -2 * M(i) * Gik << std::endl;
                    }
                }
            }

            // B terms in dP/dM_PQ
            {
                auto BSel = B.submat(0, 0, nPqPv - 1, nPq - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iM(k) == 0) std::cout << "B " << i << " " << k << std::endl;
                    double Bik = *it;
                    if (i != k)
                    {
                        if (iP(i) == 2 && iM(k) == 0) std::cout << "    A " << M(i) * Bik * sin_ik(i, k) << std::endl;
                        helper.insert(iP(i), iM(k), M(i) * Bik * sin_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dP/dM_PQ
            {
                auto GSel = G.rows(0, nPq - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iM(i) == 0) std::cout << "C" << std::endl;
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iM(i), -M(l) * Gil * cos_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dP/dM_PQ
            {
                auto BSel = B.rows(0, nPq - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iM(i) == 0) std::cout << "D" << std::endl;
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
                auto GSel = G.submat(0, 0, nPqPv - 1, nPqPv - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row();
                    uword k = it.col();
                    if (iP(i) == 2 && iT(k) == 0) std::cout << "E" << std::endl;
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iT(k), -M(i) * M(k) * Gik * sin_ik(i, k));
                    }
                }
            }

            // B terms in dP/dtheta
            {
                auto BSel = B.submat(0, 0, nPqPv - 1, nPqPv - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row();
                    uword k = it.col();
                    if (iP(i) == 2 && iT(k) == 0) std::cout << "F" << std::endl;
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iP(i), iT(k), M(i) * M(k) * Bik * cos_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dP/dtheta
            {
                auto GSel = G.rows(0, nPqPv - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iT(i) == 0) std::cout << "G" << std::endl;
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iP(i), iT(i), M(i) * M(l) * Gil * sin_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dP/dtheta
            {
                auto BSel = B.rows(0, nPqPv - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iP(i) == 2 && iT(i) == 0) std::cout << "H" << std::endl;
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
                auto GSel = G.submat(0, 0, nPq - 1, nPq - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iM(k) == 0) std::cout << "I" << std::endl;
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iM(k), -M(i) * Gik * sin_ik(i, k));
                    }
                }
            }
            
            // B terms in dQ_PQ/dM_PQ
            {
                auto BSel = B.submat(0, 0, nPq - 1, nPq - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iM(k) == 0) std::cout << "J" << std::endl;
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iM(k), M(i) * Bik * cos_ik(i, k));
                    }
                    else
                    {
                        helper.insert(iQ(i), iM(k), 2 * M(i) * Bik);
                    }
                }
            }

            // Inner sum G terms in dQ_PQ/dM_PQ
            {
                auto GSel = G.rows(0, nPq - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iM(i) == 0) std::cout << "K" << std::endl;
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iM(i), -M(l) * Gil * sin_ik(i, l));
                    }
                }
            }
 
            // Inner sum B terms in dQ_PQ/dM_PQ
            {
                auto BSel = B.rows(0, nPq - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iM(i) == 0) std::cout << "L" << std::endl;
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
                auto GSel = G.submat(0, 0, nPq - 1, nPqPv - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iT(k) == 0) std::cout << "M" << std::endl;
                    double Gik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iT(k), M(i) * M(k) * Gik * cos_ik(i, k));
                    }
                }
            }

            // B terms in dQ_PQ/dtheta
            {
                auto BSel = B.submat(0, 0, nPq - 1, nPqPv - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword k = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iT(k) == 0) std::cout << "N" << std::endl;
                    double Bik = *it;
                    if (i != k)
                    {
                        helper.insert(iQ(i), iT(k), M(i) * M(k) * Bik * sin_ik(i, k));
                    }
                }
            }

            // Inner sum G terms in dQ_PQ/dtheta
            {
                auto GSel = G.rows(0, nPq - 1);
                for (auto it = GSel.begin(); it != GSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iT(i) == 0) std::cout << "O" << std::endl;
                    double Gil = *it;
                    if (i != l)
                    {
                        helper.insert(iQ(i), iT(i), -M(i) * M(l) * Gil * cos_ik(i, l));
                    }
                }
            }

            // Inner sum B terms in dQ_PQ/dtheta
            {
                auto BSel = B.rows(0, nPq - 1);
                for (auto it = BSel.begin(); it != BSel.end(); ++it)
                {
                    uword i = it.row(); // Index in full set of busses.
                    uword l = it.col(); // Index in full set of busses.
                    if (iQ(i) == 2 && iT(i) == 0) std::cout << "P" << std::endl;
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
        // Model indexing is [0 ... nPq - 1] = PQ, [nPq ... nPq + nPv - 1] = PV, [nPq + nPv ... nPq + nPv + nSl] = SL

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

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;
        bool ok;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogMessage(LogLevel::VERBOSE) << "iter " << niter << std::endl;
            LogIndent _;

            auto S = calcS(Scg, Ic, V, M, Y);
            arma::Col<double> f = join_vert(real(S.subvec(0, nPqPv - 1)), imag(S.subvec(0, nPq - 1)));
            
            err = norm(f, "inf");
            sgtLogMessage(LogLevel::VERBOSE) << "Err = " << err << std::endl;
            if (err <= tol_) 
            {
                wasSuccessful = true;
                break;
            }

            Col<double> x; // Delta theta.
            Stopwatch sw;
            sw.start();
            SpMat<double> J = calcJ(nPq, nPv, M, theta, G, B);
            sw.stop();
            std::cout << "J " << sw.seconds() << std::endl;
            sw.reset();
            sw.start();
            SpMat<double> J1 = calcJ1(nPq, nPv, V, Y);
            sw.stop();
            std::cout << "J1 " << sw.seconds() << std::endl;

            ok = solveSparseSystem(J1, f, x);
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }
            sgtLogDebug() << "--------------------" << std::endl;
            sgtLogDebug() << "V = " << V << std::endl;
            sgtLogDebug() << "S = " << S << std::endl;
            sgtLogDebug() << "f = " << f << std::endl;
            sgtLogMessage() << "|f| = " << err << std::endl;
            sgtLogDebug() << "x = " << x << std::endl;
            sgtLogDebug() << "J =\n" << std::setw(10) << arma::Mat<double>(J) << std::endl;
            sgtLogDebug() << "J1 =\n" << std::setw(10) << arma::Mat<double>(J1) << std::endl;
            sgtLogDebug() << "--------------------" << std::endl;

            // Update M, theta, V.
            M.subvec(0, nPq - 1) += x.subvec(0, nPq - 1);
            theta.subvec(0, nPqPv - 1) += x.subvec(nPq, 2 * nPq + nPv - 1);
            V.subvec(0, nPqPv - 1) = M.subvec(0, nPqPv - 1) % exp(Complex{0, 1} * theta.subvec(0, nPqPv - 1));
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowFdSolver: failed to converge." << std::endl;
            // TODO.
        }

        return wasSuccessful;
    }
}
