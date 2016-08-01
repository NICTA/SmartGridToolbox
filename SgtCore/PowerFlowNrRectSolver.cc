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
#include "PowerFlowNrRectSolver.h"
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
    template<typename T, typename U> void initJcBlock(const T& Y, U& JVr, U& JVi)
    {
        JVr = -Y;
        JVi =  Complex(0, -1) * Y;
    }
}

namespace Sgt
{
    namespace
    {
        Col<Complex> calcSGenSl(
                span selSl,
                const Col<Complex>& V,
                const Col<double>& M,
                const SpMat<Complex>& YConst,
                const SpMat<Complex>& IConst,
                const SpMat<Complex>& SConst)
        {
            auto VSl = V(selSl);
            auto YConstSl = YConst.rows(selSl.a, selSl.b);
            auto IConstSl = IConst.rows(selSl.a, selSl.b);
            auto SConstSl = SConst.rows(selSl.a, selSl.b);

            Col<Complex> SGenSl = conj(YConst.rows(selSl.a, selSl.b) * V);

            for (auto it = SConstSl.begin(); it != SConstSl.end(); ++it)
            {
                uword i = it.row(); // Index in submatrix.
                uword k = it.col(); // Index in both submatrix and full matrix, since we're using rows().
                Complex Vik = i == k ? VSl(i) : VSl(i) - V(k); // Ground elems stored in diagonal elems of SConst.
                SGenSl(i) += static_cast<Complex>(*it) / Vik;
            }

            for (auto it = IConstSl.begin(); it != IConstSl.end(); ++it)
            {
                uword i = it.row(); // Index in submatrix.
                uword k = it.col(); // Index in both submatrix and full matrix, since we're using rows().
                Complex Vik = i == k ? VSl(i) : VSl(i) - V(k); // Ground elems stored in diagonal elems of SConst.
                SGenSl(i) += conj(IConst(i, k) * Vik) / abs(Vik);
            }

            SGenSl %= VSl;

            return SGenSl;
        }
    }

    Jacobian::Jacobian(uword nPq, uword nPv)
    {
        uword nPqPv = nPq + nPv;
        dDdVr = SpMat<Complex>(nPqPv, nPqPv);
        dDdVi = SpMat<Complex>(nPqPv, nPqPv);
        dDPvdQPv = SpMat<Complex>(nPv, nPv);
    }

    bool PowerFlowNrRectSolver::solve(Network& netw)
    {
        sgtLogDebug() << "PowerFlowNrRectSolver : solve." << std::endl;
        LogIndent indent;

        netw_ = &netw;
        bool ok = true;
        for (auto& island : netw.islands())
        {
            if (island.isSupplied)
            {
                ok = solveForIsland(island.idx) && ok;
            }
        }
        return ok;
    }

    void PowerFlowNrRectSolver::init(int islandIdx)
    {
        mod_ = buildModel(*netw_, [islandIdx](const Bus& b){return b.islandIdx() == islandIdx;});

        selDrFrom_f_.set_size(mod_->nPqPv());
        selDiFrom_f_.set_size(mod_->nPqPv());

        selVrOrQFrom_x_.set_size(mod_->nPqPv());
        selViFrom_x_.set_size(mod_->nPqPv());

        for (uword i = 0; i < mod_->nPqPv(); ++i)
        {
            selDrFrom_f_[i] = 2 * i + 1;
            selDiFrom_f_[i] = 2 * i;
        }

        for (uword i = 0; i < mod_->nPqPv(); ++i)
        {
            selVrOrQFrom_x_[i] = 2 * i;
            selViFrom_x_[i] = 2 * i + 1;
        }
    }
    
    bool PowerFlowNrRectSolver::solveForIsland(int islandIdx)
    {
        sgtLogDebug() << "Solve for island " << islandIdx << std::endl;

        double duration = 0;

        Stopwatch stopwatch;
        stopwatch.reset();
        stopwatch.start();

        // Construct the model from the network.
        init(islandIdx);

        // Variables and derived quantities:
        Col<Complex> V = mod_->V();
        Col<double> Vr = real(V);
        Col<double> Vi = imag(V);
        Col<double> M2 = square(Vr) + square(Vi);
        Col<double> M = sqrt(M2);
        Col<Complex> SGenPv = mod_->nPv() > 0 ? mod_->SGen()(mod_->selPv()) : Col<Complex>();
        Col<double> QGenPv = imag(SGenPv); // Variable.

        // Setpoints.
        const Col<double> PGenPvConst = real(SGenPv);
        const Col<double> M2PvConst = mod_->nPv() > 0 ? square(M(mod_->selPv())) : Col<double>();

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;

        auto debugPrintVars = [&](){
            if (debugLogLevel() >= LogLevel::VERBOSE)
            {
                sgtLogDebug() << "Vr = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
                sgtLogDebug() << "Vi = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
                if (mod_->nPv() > 0)
                {
                    auto ScgPv = SGenPv - Col<Complex>(mod_->SConst().diag())(mod_->selPv());
                    Col<double> PcgPv = real(ScgPv);
                    Col<double> QcgPv = imag(ScgPv);
                    sgtLogDebug() << "PcgPv = " << std::setprecision(5) << std::setw(9) << PcgPv << std::endl;
                    sgtLogDebug() << "QcgPv = " << std::setprecision(5) << std::setw(9) << QcgPv << std::endl;
                }
            }
        };

        auto debugPrintProb = [&](const auto& D, const auto& J){
            if (debugLogLevel() >= LogLevel::VERBOSE)
            {
                if (mod_->nPq() > 0)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << "DPq" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << static_cast<Mat<double>>(real(D(mod_->selPq()))) << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << static_cast<Mat<double>>(imag(D(mod_->selPq()))) << std::endl;
                }
                
                if (mod_->nPv() > 0)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << "DPv" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << static_cast<Mat<double>>(real(D(mod_->selPv()))) << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << static_cast<Mat<double>>(imag(D(mod_->selPv()))) << std::endl;
                }

                if (mod_->nPq() > 0)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << "DPq VrPq" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVr(mod_->selPq(), mod_->selPq())));
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVr(mod_->selPq(), mod_->selPq())));

                    sgtLogDebug(LogLevel::VERBOSE) << "DPq ViPq" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVi(mod_->selPq(), mod_->selPq())));
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVi(mod_->selPq(), mod_->selPq())));

                    if (mod_->nPv() > 0)
                    {
                        sgtLogDebug(LogLevel::VERBOSE) << "DPq VrPv" << std::endl;
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVr(mod_->selPq(), mod_->selPv())));
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVr(mod_->selPq(), mod_->selPv())));

                        sgtLogDebug(LogLevel::VERBOSE) << "DPq ViPv" << std::endl;
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVi(mod_->selPq(), mod_->selPv())));
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVi(mod_->selPq(), mod_->selPv())));
                    }
                }

                if (mod_->nPv() > 0)
                {
                    if (mod_->nPq() > 0)
                    {
                        sgtLogDebug(LogLevel::VERBOSE) << "DPv VrPq" << std::endl;
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVr(mod_->selPv(), mod_->selPq())));
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVr(mod_->selPv(), mod_->selPq())));

                        sgtLogDebug(LogLevel::VERBOSE) << "DPv ViPq" << std::endl;
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVi(mod_->selPv(), mod_->selPq())));
                        sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVi(mod_->selPv(), mod_->selPq())));
                    }

                    sgtLogDebug(LogLevel::VERBOSE) << "DPv VrPv" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVr(mod_->selPv(), mod_->selPv())));
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVr(mod_->selPv(), mod_->selPv())));

                    sgtLogDebug(LogLevel::VERBOSE) << "DPv ViPv" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDdVi(mod_->selPv(), mod_->selPv())));
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDdVi(mod_->selPv(), mod_->selPv())));

                    sgtLogDebug(LogLevel::VERBOSE) << "DPv QPv" << std::endl;
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(real(J.dDPvdQPv));
                    sgtLogDebug(LogLevel::VERBOSE) << Mat<double>(imag(J.dDPvdQPv));
                }
            }
        };

        auto debugPrintFandJMat = [](const auto& f, const auto& JMat){
            if (debugLogLevel() >= LogLevel::VERBOSE)
            {
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: f      = " << std::setprecision(5) << std::setw(9) << f << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: JMat   = " << std::setprecision(5) << std::setw(9) << JMat << std::endl;
            }
        };

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogDebug() << "Iteration = " << niter << std::endl;
            debugPrintVars();

            auto D = calcD(V, SGenPv, M2PvConst);

            err = norm(D, "inf");
            sgtLogDebug() << "Error = " << err << std::endl;
            if (err <= tol_)
            {
                sgtLogDebug() << "Success at iteration " << niter << "." << std::endl;
                wasSuccessful = true;
                break;
            }

            auto J = calcJ(V, SGenPv, M2PvConst);

            sgtLogDebug() << "Before modifyForPv:" << std::endl;
            debugPrintProb(D, J);

            if (mod_->nPv() > 0)
            {
                modifyForPv(J, D, Vr, Vi, M2, M2PvConst);
            }
            
            sgtLogDebug() << "After modifyForPv:" << std::endl;
            debugPrintProb(D, J);

            // Construct the f vector consisting of real and imgaginary parts of D.
            auto f = construct_f(D); 

            // Construct the full Jacobian from J, which contains the block structure.
            auto JMat = constructJMatrix(J);

            sgtLogDebug() << "Before solve:" << std::endl;
            debugPrintFandJMat(f, JMat);
            
            // Solution vector.
            Col<double> x;
            
            bool ok;
#ifdef WITH_KLU
            ok = kluSolve(JMat, -f, x);
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

            // Update the current values of V from the solution:
            if (mod_->nPq() > 0)
            {
                Vr(mod_->selPq()) += x(selVrOrQFrom_x_(mod_->selPq()));
                Vi(mod_->selPq()) += x(selViFrom_x_(mod_->selPq()));
            }

            // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
            if (mod_->nPv() > 0)
            {
                auto VrPv = Vr(mod_->selPv());
                auto ViPv = Vi(mod_->selPv());
                const auto DeltaViPv = x(selViFrom_x_(mod_->selPv()));
                VrPv += (M2PvConst - square(VrPv) - square(ViPv) - 2 * ViPv % DeltaViPv) / (2 * VrPv);
                ViPv += DeltaViPv;

                // Update QGen for PV buses based on the solution.
                QGenPv += x(selVrOrQFrom_x_(mod_->selPv()));
            }

            // Set V, M2 and M from Vr and Vi.
            V = cx_vec(Vr, Vi); 
            M2 = square(Vr) + square(Vi);
            M = sqrt(M2);
       
            // Set SGenPv from PGenPv and QGenPv.
            SGenPv = cx_vec(PGenPvConst, QGenPv);

        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowNrRectSolver: failed to converge." << std::endl;
        }

        // Propagate V to the model.
        mod_->setV(V);

        // Propagate SGen to the model.
        Col<Complex> SGen(mod_->nNode(), fill::zeros);
        if (mod_->nPv() > 0)
        {
            SGen(mod_->selPv()) = SGenPv;
        }
        if (mod_->nSl() > 0)
        {
            SGen(mod_->selSl()) = 
                calcSGenSl(mod_->selSl(), V, abs(V), mod_->YConst(), mod_->IConst(), mod_->SConst());
        }
        mod_->setSGen(SGen);

        stopwatch.stop();
        duration = stopwatch.seconds();

        sgtLogDebug() << "PowerFlowNrRectSolver: " << std::endl; 
        {
            LogIndent indent;
            sgtLogDebug() << "successful = " << wasSuccessful << std::endl;
            sgtLogDebug() << "error = " << err << std::endl; 
            sgtLogDebug() << "iterations = " << niter << std::endl;
            sgtLogDebug() << "total time = " << duration << std::endl;
        }

        if (wasSuccessful)
        {
            applyModel(*mod_, *netw_);
        }

        return wasSuccessful;
    }

    // At this stage, we are treating f as if all buses were PQ. PV buses will be taken into account later.
    Col<Complex> PowerFlowNrRectSolver::calcD(
            const Col<Complex>& V,
            const Col<Complex>& SGenPv,
            const Col<double>& M2PvConst) const
    {
        const auto YConstPqPv = mod_->YConst().rows(mod_->selPqPv().a, mod_->selPqPv().b);
        const auto IConstPqPv = mod_->IConst().rows(mod_->selPqPv().a, mod_->selPqPv().b);
        const auto SConstPqPv = mod_->SConst().rows(mod_->selPqPv().a, mod_->selPqPv().b);

        Col<Complex> D = -YConstPqPv * V;

        for (auto it = SConstPqPv.begin(); it != SConstPqPv.end(); ++it) 
        {
            uword i = it.row();
            uword k = it.col();
            if (i == k)
            {
                continue;
            }
            D(i) -= conj(*it / (V(i) - V(k)));
        }

        for (uword i = 0; i < mod_->nPq(); ++i)
        {
            uword iPq = mod_->iPq(i);
            D(iPq) -= conj(SConstPqPv(iPq, iPq) / V(iPq));
        }

        for (uword i = 0; i < mod_->nPv(); ++i)
        {
            uword iPv= mod_->iPv(i);
            D(iPv) += conj(SGenPv(i) - SConstPqPv(iPv, iPv)) * V(iPv) / M2PvConst(i);
        }

        for (auto it = IConstPqPv.begin(); it != IConstPqPv.end(); ++it) 
        {
            uword i = it.row();
            uword k = it.col();
            Complex Vik = i == k ? V(i) : V(i) - V(k); // Ground elems stored in diagonal elems of SConst.
            D(i) -= *it * Vik / abs(Vik);
        }

        return D;
    }

    // At this stage, we are treating f as if all buses were PQ. PV buses will be taken into account later.
    Jacobian PowerFlowNrRectSolver::calcJ(
            const Col<Complex>& V,
            const Col<Complex>& SGenPv,
            const Col<double>& M2PvConst) const
    {
        auto YConstPqPv = mod_->YConst()(mod_->selPqPv(), mod_->selPqPv());
        auto IConstPqPv = mod_->IConst()(mod_->selPqPv(), mod_->selPqPv());
        auto SConstPqPv = mod_->SConst()(mod_->selPqPv(), mod_->selPqPv());

        Jacobian J(mod_->nPq(), mod_->nPv());

        SparseHelper<Complex> hVr(mod_->nPqPv(), mod_->nPqPv(), true, true, true);
        SparseHelper<Complex> hVi(mod_->nPqPv(), mod_->nPqPv(), true, true, true);
        SparseHelper<Complex> hQg(mod_->nPv(), mod_->nPv(), true, true, true);

        for (auto it = SConstPqPv.begin(); it != SConstPqPv.end(); ++it)
        {
            uword i = it.row();
            uword k = it.col();
            if (i == k)
            {
                continue;
            }

            Complex Vik = V(i) - V(k);

            Complex x = conj(static_cast<Complex>(*it) / (Vik * Vik));
            Complex imX = im * x;

            hVr.insert(i, i, x);
            hVi.insert(i, i, -imX);
            hVr.insert(i, k, -x);
            hVr.insert(i, k, imX);
        }

        for (uword i = 0; i < mod_->nPq(); ++i)
        {
            uword iPq = mod_->iPq(i);
            Complex x = conj(static_cast<Complex>(SConstPqPv(iPq, iPq)) / (V(iPq) * V(iPq)));

            hVr.insert(iPq, iPq, x);
            hVi.insert(iPq, iPq, -im * x);
        }

        for (uword i = 0; i < mod_->nPv(); ++i)
        {
            uword iPv = mod_->iPv(i);
            Complex x = conj(SGenPv(i) - static_cast<Complex>(SConstPqPv(iPv, iPv))) / M2PvConst(i);

            hVr.insert(iPv, iPv, x);
            hVi.insert(iPv, iPv, im * x);
            hQg.insert(i, i, -im * V(iPv) / M2PvConst(i));
        }

        for (auto it = YConstPqPv.begin(); it != YConstPqPv.end(); ++it)
        {
            Complex x = -static_cast<Complex>(*it);
            hVr.insert(it.row(), it.col(), x);
            hVi.insert(it.row(), it.col(), im * x);
        }

        for (auto it = IConstPqPv.begin(); it != IConstPqPv.end(); ++it)
        {
            uword i = it.row();
            uword k = it.col();
            Complex Vik = i == k ? V(i) : V(i) - V(k);
            Complex Mik = abs(Vik);
            Complex M2ik = Mik * Mik;
            Complex M3ik = Mik * M2ik;

            Complex x = static_cast<Complex>(*it) * (-M2ik + real(Vik) * Vik) / M3ik;
            Complex imX = im * x;
            hVr.insert(i, i, x);
            hVi.insert(i, i, imX);
            if (i != k)
            {
                hVr.insert(i, k, -x);
                hVi.insert(i, k, -imX);
            }
        }
        J.dDdVr = hVr.get();
        J.dDdVi = hVi.get();
        J.dDPvdQPv = hQg.get();

        return J;
    }

    // Modify J and f to take into account PV buses.
    void PowerFlowNrRectSolver::modifyForPv(
            Jacobian& J,
            Col<Complex>& D,
            const Col<double>& Vr,
            const Col<double>& Vi,
            const Col<double>& M2,
            const Col<double>& M2PvConst)
    {
        for (uword k = 0; k < mod_->nPv(); ++k) // Loop over PV buses.
        {
            uword kPv = mod_->iPv(k); // Get bus index in full list of buses.
            double DMult = 0.5 * (M2PvConst(k) - M2(kPv)) / Vr(kPv);
            double JMult = -Vi(kPv) / Vr(kPv);

            auto colK = J.dDdVr.col(kPv); // Select column in J corresponding to d/dVr for PV bus k.
            for (auto it = colK.begin(); it != colK.end(); ++it)
            {
                // Loop over all buses in this column.
                uword i = it.row();
                Complex x = *it;
                D(i) += x * DMult; // Adding DMult * this column to D.
                J.dDdVi(i, kPv) += x * JMult; // Adding JMult * this column to corresponding d/dVi col.
            }
        }
    }
            
    Col<double> PowerFlowNrRectSolver::construct_f(const Col<Complex>&D) const
    {
        // Construct the real f vector from D.
        Col<double> f(nVar(), fill::none);
        for (uword i = 0; i < mod_->nPqPv(); ++i)
        {
            f(selDrFrom_f_(i)) = real(D(i));
            f(selDiFrom_f_(i)) = imag(D(i));
        }
        return f;
    }

    SpMat<double> PowerFlowNrRectSolver::constructJMatrix(const Jacobian& J) const
    {
        SparseHelper<double> h(nVar(), nVar(), true, true, true);

        auto insert = [&](const auto& JSel, const auto& indDim1R, const auto& indDim1I, const auto& indDim2)
        {
            for (auto it = JSel.begin(); it != JSel.end(); ++it)
            {
                uword i = it.row(); 
                uword k = it.col(); 
                Complex x = *it;
                h.insert(indDim1R(i), indDim2(k), real(x));
                h.insert(indDim1I(i), indDim2(k), imag(x));
            }
        };
       
        insert(J.dDdVi, selDrFrom_f_, selDiFrom_f_, selViFrom_x_);
        if (mod_->nPq() > 0)
        {
            insert(J.dDdVr.cols(mod_->selPq().a, mod_->selPq().b), selDrFrom_f_, selDiFrom_f_,
                   selVrOrQFrom_x_(mod_->selPq())); // Vr PQ.
        }
        if (mod_->nPv() > 0)
        {
            insert(J.dDPvdQPv, selDrFrom_f_(mod_->selPv()), selDiFrom_f_(mod_->selPv()), 
                   selVrOrQFrom_x_(mod_->selPv())); // Q PV.
        }

        return h.get();
    }
}
