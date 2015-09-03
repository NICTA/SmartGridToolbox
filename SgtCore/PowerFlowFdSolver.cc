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
#include "Stopwatch.h"

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
        void calcf(Col<double>& f,
                   const uword nPqPv,
                   const Col<double>& P, const Col<double>& Q,
                   const Col<double>& Irc, const Col<double>& Iic,
                   const Col<double>& M, const Col<double>& theta,
                   const SpMat<double>& G, const SpMat<double>& B)
        {
            const Col<double> cosTheta = cos(theta);
            const Col<double> sinTheta = sin(theta);

            Col<double> P = P + Irc % M;
            Col<double> Q = Q - Iic % M;
           
            // TODO: many of the trig multiplications below will end up being done twice.

            for (SpMat<double>::iterator it = G.begin(); it != G.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                double Gcos = *it * (cosTheta(i) * cosTheta(k) + sinTheta(i) * sinTheta(k));
                double Gsin = *it * (sinTheta(i) * cosTheta(k) - cosTheta(i) * sinTheta(k));
                P(i) -= M(i) * Gcos * M(k); 
                Q(i) -= M(i) * Gsin * M(k); 
            }
            
            for (SpMat<double>::iterator it = B.begin(); it != B.end(); ++it)
            {
                uword i = it.row();
                uword k = it.col();
                double Bcos = *it * (cosTheta(i) * cosTheta(k) + sinTheta(i) * sinTheta(k));
                double Bsin = *it * (sinTheta(i) * cosTheta(k) - cosTheta(i) * sinTheta(k));
                P(i) -= M(i) * Bsin * M(k); 
                Q(i) += M(i) * Bcos * M(k); 
            }

            f.subvec(0, nPqPv - 1) = P.subvec(1, nPqPv);
            f.subvec(nPqPv, 2 * nPqPv - 1) = Q.subvec(1, nPqPv);
        }
    }

    bool PowerFlowFdSolver::solveProblem()
    {
        sgtLogDebug() << "PowerFlowFdSolver : solve." << std::endl;
        LogIndent indent;

        // Set up data structures for the calculation.
        // Model indexing is 0 = slack, [1 ... nPq] = PQ, [nPq + 1 ... nPq + nPv] = PV.
        
        SpMat<double> G = real(mod_->Y()); // Model indexing. Includes shunts (const Y in ZIPs).
        SpMat<double> B = real(mod_->Y()); // Model indexing. Includes shunts (const Y in ZIPs).

        Col<double> M = abs(mod_->V()); // Model indexing.
        
        Col<double> theta(mod_->nNode(), fill::none); // Model indexing.
        for (uword i = 0; i < mod_->nNode(); ++i) theta(i) = std::abs(mod_->V()(i));
        
        Col<double> P = real(mod_->S()); // Model indexing. P = P_c + P_g.
        Col<double> Q = imag(mod_->S()); // Model indexing. Q = Q_c + Q_g.
        
        Col<double> Irc = real(mod_->IConst()); // Model indexing. P_c + P_g.
        Col<double> Iic = imag(mod_->IConst()); // Model indexing. P_c + P_g.

        Col<double> f(nVar()); // Power mismatch function, [P, Q].

        // Jacobian:
        SpMat<double> JP(nVar(), nVar());
        SpMat<double> JQ(nVar(), nVar());

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;

        // Build the Jacobian for P, theta:
        {
            SparseHelper<double> helper(nPqPv(), nPqPv(), false, false, false);
            for (uword i = 0; i < nPqPv(); ++i)
            {
                for (uword k = 0; k < i; ++k)
                {
                    double MiMk = M(i) * M(k);
                    helper.insert(i, k, MiMk * B(i, k));
                    helper.insert(k, i, MiMk * B(k, i)); // B not nec. symmetric.
                }
            }
            JP += helper.get();
        }

        // Build the Jacobian for Q, M/Qg:
        {
            SparseHelper<double> helper(nVar(), nVar(), false, false, false);
            for (uword i = 0; i < nPqPv(); ++i)
            {
                // Note: shunt terms have already been absorbed into Y (PowerFlowModel).
                for (uword k = 0; k < nPq(); ++k)
                {
                    double val = M(i) * B(i, k); if (i == k) val *= 2;
                    helper.insert(i, k, val);
                }
            }
            for (uword i = 0; i < nPv(); ++i)
            {
                helper.insert(i, i, 1.0);
            }
            JQ += helper.get();
        }

        // Do the iterations for P:
        for (niter = 0; niter < maxiter_; ++niter)
        {
            calcf(f, nPqPv(), P, Q, Irc, Iic, M, theta, G, B);

            err = norm(f, "inf");
            sgtLogDebug(LogLevel::VERBOSE) << "f  = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            sgtLogDebug() << "Error = " << err << std::endl;
            if (err <= tol_)
            {
                sgtLogDebug() << "Success at iteration " << niter << "." << std::endl;
                wasSuccessful = true;
                break;
            }

            Col<double> x;
            bool ok;
#ifdef WITH_KLU
            ok = kluSolve(JP, -f, x);
#else
            ok = spsolve(x, JMat, -f, "superlu");
#endif
            sgtLogDebug() << "After solve: ok = " << ok << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) 
                << "After solve: x  = " << std::setprecision(5) << std::setw(9) << x << std::endl;
            if (!ok)
            {
                sgtLogWarning() << "Solve failed." << std::endl;
                break;
            }

            // Update the current variables (M, theta, Q) from the solution:
            if (mod_->nPq() > 0)
            {
                M.subvec(1, nPq()) += x.subvec(0, nPq() - 1);
            }
            if (mod_->nPv() > 0)
            {
                Q.subvec(nPq() + 1, nPqPv()) += x.subvec(nPq(), nPqPv() - 1);
            }
            theta.subvec(1, nPqPv()) += x.subvec(nPqPv(), nVar() - 1);
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowFdSolver: Newton-Raphson method failed to converge." << std::endl;
            for (std::size_t i = 0; i < mod_->nNode(); ++i)
            {
                auto node = mod_->nodes()[i];
                node->V_ = 0;
                node->S_ = 0;
                node->bus_->V_[node->phaseIdx_] = node->V_;
                node->bus_->S_[node->phaseIdx_] = node->S_;
            }
        }

        for (uword i = 0; i < mod_->nNode(); ++i)
        {
            mod_->V()(i) = {Vr(i), Vi(i)};
            mod_->S()(i) = {P(i), Q(i)};
        }

        // Set the slack power.
        if (mod_->nSl() > 0)
        {
            auto SSl = mod_->S()(mod_->selSlFromAll());

            auto VSl = mod_->V()(mod_->selSlFromAll());
            auto IConstSl = mod_->IConst()(mod_->selSlFromAll());

            SpMat<Complex> YStar = mod_->Y()(mod_->selSlFromAll(), mod_->selAllFromAll());
            for (auto elem : YStar) elem = std::conj(Complex(elem));
            auto VStar = colConj(mod_->V());
            auto IConstStar = colConj(mod_->IConst()(mod_->selSlFromAll()));

            SSl = VSl % (YStar * VStar) - VSl % IConstStar;
        }

        // Update nodes and busses.
        for (uword i = 0; i < mod_->nNode(); ++i)
        {
            auto node = mod_->nodes()[i];
            node->V_ = mod_->V()(i);
            node->S_ = mod_->S()(i);
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
        }

        stopwatchTot.stop();
        durationTot = stopwatchTot.seconds();

        sgtLogDebug()
            << "PowerFlowFdSolver: \n"
            << "    successful = " << wasSuccessful << "\n"
            << "    error = " << err << "\n" 
            << "    iterations = " << niter << "\n"
            << "    total time = " << durationTot << "\n" 
            << "    time to create model = " << durationMakeModel << "\n"
            << "    time for setup = " << durationInitSetup << "\n"
            << "    calcf time = " << durationCalcf << "\n"
            << "    updateJ time = " << durationUpdateJ << "\n"
            << "    modifyForPv time = " << durationModifyForPv << "\n"
            << "    constructJMat time = " << durationConstructJMat << "\n"
            << "    solve time = " << durationSolve << "\n"
            << "    updateIter time = " << durationUpdateIter << std::endl;

        return wasSuccessful;
    }
}
