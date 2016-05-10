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
        void updateScgSl(
                Col<Complex>& Scg,
                arma::span selSl,
                const Col<Complex>& Ic,
                const Col<Complex>& V,
                const Col<double>& M,
                const SpMat<Complex>& Y)
        {
            Scg(selSl) = V(selSl) % conj(Y.rows(selSl.a, selSl.b) * V) - conj(Ic(selSl)) % M(selSl);
        }
    }

    Jacobian::Jacobian(uword nPq, uword nPv)
    {
        for (std::size_t i = 0; i < 2; ++i)
        {
            for (std::size_t k = 0; k < 2; ++k)
            {
                blocks_[i][k] = SpMat<double>(nPq, nPq);
            }
            for (std::size_t k = 2; k < 5; ++k)
            {
                blocks_[i][k] = SpMat<double>(nPq, nPv);
            }
        }
        for (std::size_t i = 2; i < 4; ++i)
        {
            for (std::size_t k = 0; k < 2; ++k)
            {
                blocks_[i][k] = SpMat<double>(nPv, nPq);
            }
            for (std::size_t k = 2; k < 5; ++k)
            {
                blocks_[i][k] = SpMat<double>(nPv, nPv);
            }
        }
    }

    bool PowerFlowNrRectSolver::solveProblem()
    {
        sgtLogDebug() << "PowerFlowNrRectSolver : solve." << std::endl;
        LogIndent indent;

        Stopwatch stopwatch;

        double duration = 0;

        stopwatch.reset();
        stopwatch.start();

        // Construct the model from the network.
        
        init(*netw_);

        Col<Complex> V = mod_->V(); // Cache, as model cacluates on the fly.
        Col<double> Vr = real(V);
        Col<double> Vi = imag(V);
        Col<double> M = abs(V);
        Col<double> M2Pv = mod_->nPv() > 0
            ? Vr(mod_->selPv()) % Vr(mod_->selPv()) 
              + Vi(mod_->selPv()) % Vi(mod_->selPv())
            : Col<double>(); // Constant.

        Col<Complex> Scg = mod_->Scg(); // Model indexing. S_cg = S_c + S_g. Cache, as model calculates on the fly.
        Col<double> Pcg = real(Scg);
        Col<double> Qcg = imag(Scg);

        const Col<Complex>& Ic = mod_->IConst(); // Model indexing. P_c + P_g. Cache, as model calculates on the fly.

        // Set up data structures for the calculation.
        
        G_ = real(mod_->Y()); // Constant.
        B_ = imag(mod_->Y()); // Constant.

        Jacobian Jc(mod_->nPq(), mod_->nPv()); ///< The part of J that doesn't update at each iteration.
        initJc(Jc);

        Col<double> f(nVar()); ///< Current mismatch function.

        Jacobian J = Jc; ///< Jacobian, d f_i/d x_i.

        bool wasSuccessful = false;
        double err = 0;
        unsigned int niter;

        for (niter = 0; niter < maxiter_; ++niter)
        {
            sgtLogDebug() << "Iteration = " << niter << std::endl;

            calcf(f, Vr, Vi, M, Pcg, Qcg, Ic, M2Pv);

            err = norm(f, "inf");
            sgtLogDebug(LogLevel::VERBOSE) << "f = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            sgtLogDebug() << "Error = " << err << std::endl;
            if (err <= tol_)
            {
                sgtLogDebug() << "Success at iteration " << niter << "." << std::endl;
                wasSuccessful = true;
                break;
            }

            updateJ(J, Jc, Vr, Vi, Pcg, Qcg, M2Pv);

            if (mod_->nPv() > 0)
            {
                modifyForPv(J, f, Vr, Vi, M2Pv);
            }

            // Construct the full Jacobian from J, which contains the block structure.
            SpMat<double> JMat;
            calcJMatrix(JMat, J);

            if (debugLogLevel() >= LogLevel::VERBOSE)
            {
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Before solve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (Vr % Vr + Vi % Vi) << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: Pcg   = " << std::setprecision(5) << std::setw(9) << Pcg << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: Qcg   = " << std::setprecision(5) << std::setw(9) << Qcg << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) 
                    << "Before solve: f   = " << std::setprecision(5) << std::setw(9) << f << std::endl;
                sgtLogDebug(LogLevel::VERBOSE) << "Before solve: J   = " << std::endl;
                LogIndent indent;
                for (uword i = 0; i < nVar(); ++i)
                {
                    sgtLogDebug(LogLevel::VERBOSE)
                        << std::setprecision(5) << std::setw(9) << JMat.row(i) << std::endl;
                }
            }

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
                Vr(mod_->selPq()) += x(selVrPqFrom_x_);
                Vi(mod_->selPq()) += x(selViPqFrom_x_);
            }

            // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
            if (mod_->nPv() > 0)
            {
                auto VrPv = Vr(mod_->selPv());
                auto ViPv = Vi(mod_->selPv());
                const auto DeltaViPv = x(selViPvFrom_x_);
                VrPv += (M2Pv - VrPv % VrPv - ViPv % ViPv - 2 * ViPv % DeltaViPv) / (2 * VrPv);
                ViPv += DeltaViPv;

                // Update Qcg for PV buses based on the solution.
                Qcg(mod_->selPv()) += x(selQPvFrom_x_);
            }
        
            if (mod_->nPq() > 0)
            {
                auto VrPq = Vr(mod_->selPq());
                auto ViPq = Vi(mod_->selPq());
                M(mod_->selPq()) = sqrt(VrPq % VrPq + ViPq % ViPq);
            }

            if (debugLogLevel() >= LogLevel::VERBOSE)
            {
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Updated Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Updated Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Updated M^2 = " << std::setprecision(5) << std::setw(9) << Vr % Vr + Vi % Vi << std::endl;
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Updated Pcg   = " << std::setprecision(5) << std::setw(9) << Pcg << std::endl;
                sgtLogDebug(LogLevel::VERBOSE)
                    << "Updated Qcg   = " << std::setprecision(5) << std::setw(9) << Qcg << std::endl;
            }
        }

        if (!wasSuccessful)
        {
            sgtLogWarning() << "PowerFlowNrRectSolver: failed to converge." << std::endl;
        }

        V = cx_vec(Vr, Vi);

        mod_->setV(V);

        Scg = cx_vec(Pcg, Qcg);
        if (mod_->nSl() > 0)
        {
            updateScgSl(Scg, mod_->selSl(), Ic, V, abs(V), mod_->Y());
        }
        mod_->setScg(Scg);

        stopwatch.stop();
        duration = stopwatch.seconds();

        sgtLogDebug() << "PowerFlowNrRectSolver: " << std::endl; 
        indent.in(); 
        sgtLogDebug() << "successful = " << wasSuccessful << std::endl;
        sgtLogDebug() << "error = " << err << std::endl; 
        sgtLogDebug() << "iterations = " << niter << std::endl;
        sgtLogDebug() << "total time = " << duration << std::endl; 

        return wasSuccessful;
    }

    void PowerFlowNrRectSolver::init(Network& netw)
    {
        mod_ = buildModel(*netw_);

        selIrPqFrom_f_.set_size(mod_->nPq());
        selIiPqFrom_f_.set_size(mod_->nPq());
        selIrPvFrom_f_.set_size(mod_->nPv());
        selIiPvFrom_f_.set_size(mod_->nPv());
        selVrPqFrom_x_.set_size(mod_->nPq());
        selViPqFrom_x_.set_size(mod_->nPq());
        selQPvFrom_x_.set_size(mod_->nPv());
        selViPvFrom_x_.set_size(mod_->nPv());

        for (uword i = 0; i < mod_->nPq(); ++i)
        {
            selIrPqFrom_f_[i] = 2 * i + 1;
            selIiPqFrom_f_[i] = 2 * i;
        }

        for (uword i = 0; i < mod_->nPv(); ++i)
        {
            selIrPvFrom_f_[i] = 2 * mod_->nPq() + 2 * i + 1;
            selIiPvFrom_f_[i] = 2 * mod_->nPq() + 2 * i;
        }

        for (uword i = 0; i < mod_->nPq(); ++i)
        {
            selVrPqFrom_x_[i] = 2 * i;
            selViPqFrom_x_[i] = 2 * i + 1;
        }

        for (uword i = 0; i < mod_->nPv(); ++i)
        {
            selQPvFrom_x_[i] = 2 * mod_->nPq() + 2 * i;
            selViPvFrom_x_[i] = 2 * mod_->nPq() + 2 * i + 1;
        }

        // Just in case...
        G_.reset();
        B_.reset();
    }

    /// Set the part of J that doesn't update at each iteration.
    /** At this stage, we are treating J as if all buses were PQ. */
    void PowerFlowNrRectSolver::initJc(Jacobian& Jc) const
    {
        if (mod_->nPq() > 0)
        {
            initJcBlock(G_(mod_->selPq(), mod_->selPq()),
                        B_(mod_->selPq(), mod_->selPq()),
                        Jc.IrPqVrPq(),
                        Jc.IrPqViPq(),
                        Jc.IiPqVrPq(),
                        Jc.IiPqViPq());
        }
        else
        {
            Jc.IrPqVrPq().reset();
            Jc.IrPqViPq().reset();
            Jc.IiPqVrPq().reset();
            Jc.IiPqViPq().reset();
        }

        if (mod_->nPv() > 0)
        {
            initJcBlock(G_(mod_->selPv(), mod_->selPv()),
                        B_(mod_->selPv(), mod_->selPv()),
                        Jc.IrPvVrPv(),
                        Jc.IrPvViPv(),
                        Jc.IiPvVrPv(),
                        Jc.IiPvViPv());
        }
        else
        {
            Jc.IrPvVrPv().reset();
            Jc.IrPvViPv().reset();
            Jc.IiPvVrPv().reset();
            Jc.IiPvViPv().reset();
        }

        if (mod_->nPv() > 0 && mod_->nPq() > 0)
        {
            initJcBlock(G_(mod_->selPq(), mod_->selPv()),
                        B_(mod_->selPq(), mod_->selPv()),
                        Jc.IrPqVrPv(),
                        Jc.IrPqViPv(),
                        Jc.IiPqVrPv(),
                        Jc.IiPqViPv());
            initJcBlock(G_(mod_->selPv(), mod_->selPq()),
                        B_(mod_->selPv(), mod_->selPq()),
                        Jc.IrPvVrPq(),
                        Jc.IrPvViPq(),
                        Jc.IiPvVrPq(),
                        Jc.IiPvViPq());
        }
        else
        {
            Jc.IrPqVrPv().reset();
            Jc.IrPqViPv().reset();
            Jc.IiPqVrPv().reset();
            Jc.IiPqViPv().reset();
            Jc.IrPvVrPq().reset();
            Jc.IrPvViPq().reset();
            Jc.IiPvVrPq().reset();
            Jc.IiPvViPq().reset();
        }
    }

    // At this stage, we are treating f as if all buses were PQ. PV buses will be taken into account later.
    void PowerFlowNrRectSolver::calcf(Col<double>& f,
            const Col<double>& Vr, const Col<double>& Vi, const Col<double>& M,
            const Col<double>& Pcg, const Col<double>& Qcg,
            const Col<Complex>& Ic, const Col<double>& M2Pv) const
    {
        if (mod_->nPq() > 0)
        {
            // PQ buses:
            const SpMat<double> GPq = G_(mod_->selPq(), span::all);
            const SpMat<double> BPq = B_(mod_->selPq(), span::all);

            const auto VrPq = Vr(mod_->selPq());
            const auto ViPq = Vi(mod_->selPq());
            
            const auto MPq = M(mod_->selPq());

            const auto PPq = Pcg(mod_->selPq());
            const auto QPq = Qcg(mod_->selPq());

            auto IConstPq = Ic(mod_->selPq()).eval();
            const auto IConstrPq = real(IConstPq);
            const auto IConstiPq = imag(IConstPq);

            Col<double> M2Pq = MPq % MPq;

            f(selIrPqFrom_f_) = (VrPq % PPq + ViPq % QPq) / M2Pq 
                              + (IConstrPq % VrPq - IConstiPq % ViPq) / MPq 
                              - GPq * Vr + BPq * Vi;
            f(selIiPqFrom_f_) = (ViPq % PPq - VrPq % QPq) / M2Pq 
                              + (IConstrPq % ViPq + IConstiPq % VrPq) / MPq 
                              - GPq * Vi - BPq * Vr;
        }

        if (mod_->nPv() > 0)
        {
            // PV buses. Note that these differ in that M2Pv is considered a constant.
            const auto GPv = G_(mod_->selPv(), span::all);
            const auto BPv = B_(mod_->selPv(), span::all);

            const auto VrPv = Vr(mod_->selPv());
            const auto ViPv = Vi(mod_->selPv());
            
            const auto MPv = M(mod_->selPv());

            const auto PPv = Pcg(mod_->selPv());
            const auto QPv = Qcg(mod_->selPv());

            auto IConstPv = Ic(mod_->selPv()).eval();
            const auto IConstrPv = real(IConstPv);
            const auto IConstiPv = imag(IConstPv);

            f(selIrPvFrom_f_) = (VrPv % PPv + ViPv % QPv) / M2Pv
                              + (IConstrPv % VrPv - IConstiPv % ViPv) / MPv 
                              - GPv * Vr + BPv * Vi;
            f(selIiPvFrom_f_) = (ViPv % PPv - VrPv % QPv) / M2Pv 
                              + (IConstrPv % ViPv + IConstiPv % VrPv) / MPv 
                              - GPv * Vi - BPv * Vr;
        }
    }

    // At this stage, we are treating f as if all buses were PQ. PV buses will be taken into account later.
    void PowerFlowNrRectSolver::updateJ(Jacobian& J, const Jacobian& Jc,
                                    const Col<double>& Vr, const Col<double>& Vi,
                                    const Col<double>& Pcg, const Col<double>& Qcg,
                                    const Col<double>& M2Pv) const
    {
        // Elements in J that have no non-constant part will be initialized to the corresponding term in Jc at the
        // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.

        if (mod_->nPv() > 0)
        {
            // Reset PV Vi columns, since these get messed with:
            J.IrPqViPv() = Jc.IrPqViPv();
            J.IiPqViPv() = Jc.IiPqViPv();
            J.IrPvViPv() = Jc.IrPvViPv();
            J.IiPvViPv() = Jc.IiPvViPv();
        }

        // Block diagonal:
        for (uword i = 0; i < mod_->nPq(); ++i)
        {
            uword iPqi = mod_->iPq(i);

            double PVr_p_QVi = Pcg(iPqi) * Vr(iPqi) + Qcg(iPqi) * Vi(iPqi);
            double PVi_m_QVr = Pcg(iPqi) * Vi(iPqi) - Qcg(iPqi) * Vr(iPqi);
            double M2 = Vr(iPqi) * Vr(iPqi) + Vi(iPqi) * Vi(iPqi);
            double M4 = M2 * M2;
            double VrdM4 = Vr(iPqi) / M4;
            double VidM4 = Vi(iPqi) / M4;
            double PdM2 = Pcg(iPqi) / M2;
            double QdM2 = Qcg(iPqi) / M2;

            J.IrPqVrPq()(i, i) = Jc.IrPqVrPq()(i, i) - (2 * VrdM4 * PVr_p_QVi) + PdM2;
            J.IrPqViPq()(i, i) = Jc.IrPqViPq()(i, i) - (2 * VidM4 * PVr_p_QVi) + QdM2;
            J.IiPqVrPq()(i, i) = Jc.IiPqVrPq()(i, i) - (2 * VrdM4 * PVi_m_QVr) - QdM2;
            J.IiPqViPq()(i, i) = Jc.IiPqViPq()(i, i) - (2 * VidM4 * PVi_m_QVr) + PdM2;
        }

        // For PV buses, M^2 is constant, and therefore we can write the Jacobian more simply.
        for (uword i = 0; i < mod_->nPv(); ++i)
        {
            uword iPvi = mod_->iPv(i);

            J.IrPvVrPv()(i, i) = Jc.IrPvVrPv()(i, i) + Pcg(iPvi) / M2Pv(i); // Could -> Jc if we wanted.
            J.IrPvViPv()(i, i) = Jc.IrPvViPv()(i, i) + Qcg(iPvi) / M2Pv(i);
            J.IiPvVrPv()(i, i) = Jc.IiPvVrPv()(i, i) - Qcg(iPvi) / M2Pv(i);
            J.IiPvViPv()(i, i) = Jc.IiPvViPv()(i, i) + Pcg(iPvi) / M2Pv(i);
        }

        if (mod_->nPv() > 0)
        {
            // Set the PV Qcg columns in the Jacobian. They are diagonal.
            const auto VrPv = Vr(mod_->selPv());
            const auto ViPv = Vi(mod_->selPv());
            for (uword i = 0; i < mod_->nPv(); ++i)
            {
                J.IrPvQPv()(i, i) = ViPv(i) / M2Pv(i);
                J.IiPvQPv()(i, i) = -VrPv(i) / M2Pv(i);
            }
        }
    }

    // Modify J and f to take into account PV buses.
    void PowerFlowNrRectSolver::modifyForPv(Jacobian& J, Col<double>& f,
                                        const Col<double>& Vr, const Col<double>& Vi,
                                        const Col<double>& M2Pv)
    {
        auto mod = [&f](uword k, const Col<uword>& idx, SpMat<double>& JViPv, const SpMat<double>& JVrPv,
                        double fMult, double JMult)
        {
            auto colVrPv = JVrPv.col(k);
            for (auto it = colVrPv.begin(); it != colVrPv.end(); ++it)
            {
                uword iRow = it.row();
                f(idx(iRow)) += JVrPv(iRow, k) * fMult;
                JViPv(iRow, k) += JVrPv(iRow, k) * JMult;
            }
        };

        const auto VrPv = Vr(mod_->selPv());
        const auto ViPv = Vi(mod_->selPv());

        for (uword k = 0; k < mod_->nPv(); ++k)
        {
            double fMult = (0.5 * (M2Pv(k) - VrPv(k) * VrPv(k) - ViPv(k) * ViPv(k)) / VrPv(k));
            double colViPvMult = -ViPv(k) / VrPv(k);

            if (mod_->nPq() > 0)
            {
                mod(k, selIrPqFrom_f_, J.IrPqViPv(), J.IrPqVrPv(), fMult, colViPvMult);
                mod(k, selIiPqFrom_f_, J.IiPqViPv(), J.IiPqVrPv(), fMult, colViPvMult);
            }
            mod(k, selIrPvFrom_f_, J.IrPvViPv(), J.IrPvVrPv(), fMult, colViPvMult);
            mod(k, selIiPvFrom_f_, J.IiPvViPv(), J.IiPvVrPv(), fMult, colViPvMult);
        }
    }

    void PowerFlowNrRectSolver::calcJMatrix(SpMat<double>& JMat, const Jacobian& J) const
    {
        Array<unsigned int, 4> ibInd = {{0, 1, 2, 3}};
        Array<unsigned int, 4> kbInd = {{0, 1, 3, 4}}; // Skip VrPv, since it doesn't appear as a variable.
        Array<Col<uword>, 4> sl1Vec = {{selIrPqFrom_f_, selIiPqFrom_f_, selIrPvFrom_f_, selIiPvFrom_f_}};
        Array<Col<uword>, 4> sl2Vec = {{selVrPqFrom_x_, selViPqFrom_x_, selViPvFrom_x_, selQPvFrom_x_}};

        JMat = SpMat<double>(nVar(), nVar());

        // Loop over all blocks in J.
        for (std::size_t ib = 0; ib < 4; ++ib)
        {
            Col<uword>& sl1 = sl1Vec[ib];
            for (std::size_t kb = 0; kb < 4; ++kb)
            {
                SparseHelper<double> helper(nVar(), nVar(), false, false, false);
                Col<uword>& sl2 = sl2Vec[kb];
                const SpMat<double>& block = J.blocks_[ibInd[ib]][kbInd[kb]];

                for (auto it = block.begin(); it != block.end(); ++it)
                {
                    uword i1 = sl1(it.row());
                    uword k1 = sl2(it.col());
                    helper.insert(i1, k1, *it);
                }
                JMat += helper.get();
            }
        }
    }
}
