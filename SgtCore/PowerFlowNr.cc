#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNr.h"
#include "SparseSolver.h"
#include "Stopwatch.h"

using namespace SmartGridToolbox;
using namespace arma;

namespace
{
   SpMat<double> real(const SpMat<Complex>& cMat)
   {
      Col<uword> rowInd(cMat.n_nonzero + 1, fill::none);
      for (int i = 0; i < cMat.n_nonzero + 1; ++i)
      {
         rowInd(i) = cMat.row_indices[i];
      }
      Col<uword> colPtr(cMat.n_cols + 1, fill::none);
      for (int i = 0; i < cMat.n_cols + 1; ++i)
      {
         colPtr(i) = cMat.col_ptrs[i];
      }
      Col<double> values(cMat.n_nonzero + 1, fill::none);
      for (int i = 0; i < cMat.n_nonzero + 1; ++i)
      {
         values(i) = cMat.values[i].real();
      }
      SpMat<double> result(rowInd, colPtr, values, cMat.n_rows, cMat.n_cols);
      return result;
   }

   SpMat<double> imag(const SpMat<Complex>& cMat)
   {
      Col<uword> rowInd(cMat.n_nonzero + 1, fill::none);
      for (int i = 0; i < cMat.n_nonzero + 1; ++i)
      {
         rowInd(i) = cMat.row_indices[i];
      }
      Col<uword> colPtr(cMat.n_cols + 1, fill::none);
      for (int i = 0; i < cMat.n_cols + 1; ++i)
      {
         colPtr(i) = cMat.col_ptrs[i];
      }
      Col<double> values(cMat.n_nonzero + 1, fill::none);
      for (int i = 0; i < cMat.n_nonzero + 1; ++i)
      {
         values(i) = cMat.values[i].imag();
      }
      SpMat<double> result(rowInd, colPtr, values, cMat.n_rows, cMat.n_cols);
      return result;
   }
   
   template<typename T> Col<Complex> colConj(const T& from)
   {
      Col<Complex> result(from.n_elem, fill::none);
      for (int i = 0; i < from.n_elem; ++i)
      {
         result[i] = std::conj(from[i]);
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

namespace SmartGridToolbox
{
   Jacobian::Jacobian(int nPq, int nPv)
   {
      for (int i = 0; i < 2; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k] = SpMat<double>(nPq, nPq);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k] = SpMat<double>(nPq, nPv);
         }
      }
      for (int i = 2; i < 4; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k] = SpMat<double>(nPv, nPq);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k] = SpMat<double>(nPv, nPv);
         }
      }
   }

   PowerFlowNr::PowerFlowNr(PowerFlowModel* prob) :
      mod_(prob),
      selIrPqFrom_f_(prob->nPq()),
      selIiPqFrom_f_(prob->nPq()),
      selIrPvFrom_f_(prob->nPv()),
      selIiPvFrom_f_(prob->nPv()),
      selVrPqFrom_x_(prob->nPq()),
      selViPqFrom_x_(prob->nPq()),
      selQPvFrom_x_(prob->nPv()),
      selViPvFrom_x_(prob->nPv())
   {
      initSubmatrixRanges();
   }

   bool PowerFlowNr::solve()
   {
      SGT_DEBUG(Log().debug() << "PowerFlowNr : solve." << std::endl; LogIndent _;);

      Stopwatch stopwatch;
      Stopwatch stopwatchTot;

      double durationInitSetup = 0;
      double durationCalcf = 0;
      double durationUpdateJ = 0;
      double durationModifyForPv = 0;
      double durationConstructJMat = 0;
      double durationSolve = 0;
      double durationUpdateIter = 0;
      double durationTot = 0;

      stopwatchTot.reset(); stopwatchTot.start();

      // Do the initial setup.
      stopwatch.reset(); stopwatch.start();

      const double tol = 1e-8;
      const int maxiter = 20;

      G_ = real(mod_->Y());
      B_ = imag(mod_->Y());

      Col<double> Vr = real(mod_->V());
      Col<double> Vi = imag(mod_->V());

      Col<double> P = real(mod_->S());
      Col<double> Q = imag(mod_->S());

      Col<double> M2Pv = mod_->nPv() > 0
         ? Vr(mod_->selPvFromAll()) % Vr(mod_->selPvFromAll()) + Vi(mod_->selPvFromAll()) % Vi(mod_->selPvFromAll())
         : Col<double>();

      Jacobian Jc(mod_->nPq(), mod_->nPv()); ///< The part of J that doesn't update at each iteration.
      initJc(Jc);

      Col<double> f(nVar()); ///< Current mismatch function.

      Jacobian J = Jc; ///< Jacobian, d f_i/d x_i.

      bool wasSuccessful = false;
      double err = 0;
      int niter;

      stopwatch.stop(); durationInitSetup = stopwatch.seconds();

      for (niter = 0; niter < maxiter; ++niter)
      {
         SGT_DEBUG(Log().debug() << "Iteration = " << niter << std::endl);

         stopwatch.reset(); stopwatch.start();
         calcf(f, Vr, Vi, P, Q, M2Pv);

         err = norm(f, "inf");
         SGT_DEBUG(Log().debug() << "f  = " << std::setprecision(5) << std::setw(9) << f << std::endl);
         SGT_DEBUG(Log().debug() << "Error = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(Log().debug() << "Success at iteration " << niter << "." << std::endl);
            wasSuccessful = true;
            break;
         }
         stopwatch.stop(); durationCalcf += stopwatch.seconds();

         stopwatch.reset(); stopwatch.start();
         updateJ(J, Jc, Vr, Vi, P, Q, M2Pv);
         stopwatch.stop(); durationUpdateJ += stopwatch.seconds();

         stopwatch.reset(); stopwatch.start();
         if (mod_->nPv() > 0)
         {
            modifyForPv(J, f, Vr, Vi, M2Pv);
         }
         stopwatch.stop(); durationModifyForPv += stopwatch.seconds();

         // Construct the full Jacobian from J, which contains the block structure.
         stopwatch.reset(); stopwatch.start();
         SpMat<double> JMat; calcJMatrix(JMat, J);
         stopwatch.stop(); durationConstructJMat += stopwatch.seconds();

         SGT_DEBUG
         (
            Log().debug() << "Before kluSolve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
            Log().debug() << "Before kluSolve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
            Log().debug() << "Before kluSolve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (Vr % Vr + Vi % Vi) << std::endl;
            Log().debug() << "Before kluSolve: P   = " << std::setprecision(5) << std::setw(9) << P << std::endl;
            Log().debug() << "Before kluSolve: Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl;
            Log().debug() << "Before kluSolve: f   = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            Log().debug() << "Before kluSolve: J   = " << std::endl;
            LogIndent _;
            for (int i = 0; i < nVar(); ++i)
            {
               Log().debug() << std::setprecision(5) << std::setw(9) << row(JMat, i) << std::endl;
            }
         );

         stopwatch.reset(); stopwatch.start();
         Col<double> x;
         bool ok = kluSolve(JMat, -f, x);
         stopwatch.stop(); durationSolve += stopwatch.seconds();

         SGT_DEBUG(Log().debug() << "After kluSolve: ok = " << ok << std::endl);
         SGT_DEBUG(Log().debug() << "After kluSolve: x  = " << std::setprecision(5) << std::setw(9) << x 
               << std::endl);
         if (!ok)
         {
            Log().warning() << "kluSolve failed." << std::endl;
            break;
         }

         stopwatch.reset(); stopwatch.start();
         // Update the current values of V from the solution:
         if (mod_->nPq() > 0)
         {
            Vr(mod_->selPqFromAll()) += x(selVrPqFrom_x_);
            Vi(mod_->selPqFromAll()) += x(selViPqFrom_x_);
         }

         // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
         if (mod_->nPv() > 0)
         {
            auto VrPv = Vr(mod_->selPvFromAll());
            auto ViPv = Vi(mod_->selPvFromAll());
            const auto DeltaViPv = x(selViPvFrom_x_);
            VrPv += (M2Pv - VrPv % VrPv - ViPv % ViPv - 2 * ViPv % DeltaViPv) / (2 * VrPv);
            ViPv += DeltaViPv;

            // Update Q for PV busses based on the solution.
            Q(mod_->selPvFromAll()) += x(selQPvFrom_x_);
         }

         SGT_DEBUG(Log().debug() << "Updated Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl);
         SGT_DEBUG(Log().debug() << "Updated Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl);
         SGT_DEBUG(Log().debug() << "Updated M^2 = " << std::setprecision(5) << std::setw(9)
                           << Vr % Vr + Vi % Vi << std::endl);
         SGT_DEBUG(Log().debug() << "Updated P   = " << std::setprecision(5) << std::setw(9) << P << std::endl);
         SGT_DEBUG(Log().debug() << "Updated Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl);
         stopwatch.stop(); durationUpdateIter += stopwatch.seconds();
      }

      if (wasSuccessful)
      {
         for (int i = 0; i < mod_->nNode(); ++i)
         {
            mod_->V()(i) = {Vr(i), Vi(i)};
            mod_->S()(i) = {P(i), Q(i)};
         }

         // Set the slack power.
         if (mod_->nSl() > 0)
         {
            auto SSl = mod_->S()(mod_->selSlFromAll());

            auto VSl = mod_->V()(mod_->selSlFromAll());
            auto IZipSl = mod_->IZip()(mod_->selSlFromAll());

            SpMat<Complex> YStar = mod_->Y()(mod_->selSlFromAll(), mod_->selAllFromAll());
            for (auto elem : YStar) elem = std::conj(Complex(elem));
            auto VStar = colConj(mod_->V());
            auto IZipStar = colConj(mod_->IZip()(mod_->selSlFromAll()));

            SSl = VSl % (YStar * VStar) - VSl % IZipStar;
         }

         // Update nodes and busses.
         for (int i = 0; i < mod_->nNode(); ++i)
         {
            auto node = mod_->nodes()[i];
            node->V_ = mod_->V()(i);
            node->S_ = mod_->S()(i);
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
         }
      }
      else
      {
         Log().warning() << "PowerFlowNr: Newton-Raphson method failed to converge." << std::endl;
         for (int i = 0; i < mod_->nNode(); ++i)
         {
            auto node = mod_->nodes()[i];
            node->V_ = 0;
            node->S_ = 0;
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
         }
      }

      stopwatchTot.stop(); durationTot = stopwatchTot.seconds();

      SGT_DEBUG(Log().message() << "PowerFlowNr: successful = " << wasSuccessful << ", error = " << err
            << ", iterations = " << niter << ", total time = " << durationTot << "." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: -----------------------   " << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: init setup time         = " << durationInitSetup << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in calcf           = " << durationCalcf << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in updateJ         = " << durationUpdateJ << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in modifyForPv     = " << durationModifyForPv << " s." 
            << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time to construct JMat  = " << durationConstructJMat << " s." 
            << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: solve time              = " << durationSolve << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time to update iter     = " << durationUpdateIter << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: -----------------------   " << std::endl);

      return wasSuccessful;
   }

   void PowerFlowNr::initSubmatrixRanges()
   {
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
   }

   /// Set the part of J that doesn't update at each iteration.
   /** At this stage, we are treating J as if all busses were PQ. */
   void PowerFlowNr::initJc(Jacobian& Jc) const
   {
      if (mod_->nPq() > 0)
      {
         initJcBlock(G_(mod_->selPqFromAll(), mod_->selPqFromAll()), B_(mod_->selPqFromAll(), mod_->selPqFromAll()),
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
         initJcBlock(G_(mod_->selPvFromAll(), mod_->selPvFromAll()), B_(mod_->selPvFromAll(), mod_->selPvFromAll()),
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
         initJcBlock(G_(mod_->selPqFromAll(), mod_->selPvFromAll()), B_(mod_->selPqFromAll(), mod_->selPvFromAll()),
                     Jc.IrPqVrPv(),
                     Jc.IrPqViPv(),
                     Jc.IiPqVrPv(),
                     Jc.IiPqViPv());
         initJcBlock(G_(mod_->selPvFromAll(), mod_->selPqFromAll()), B_(mod_->selPvFromAll(), mod_->selPqFromAll()),
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

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::calcf(Col<double>& f,
                           const Col<double>& Vr, const Col<double>& Vi,
                           const Col<double>& P, const Col<double>& Q,
                           const Col<double>& M2Pv) const
   {
      if (mod_->nPq() > 0)
      {
         // PQ busses:
         const SpMat<double> GPq = G_(mod_->selPqFromAll(), mod_->selAllFromAll());
         const SpMat<double> BPq = B_(mod_->selPqFromAll(), mod_->selAllFromAll());

         const auto VrPq = Vr(mod_->selPqFromAll());
         const auto ViPq = Vi(mod_->selPqFromAll());

         const auto PPq = P(mod_->selPqFromAll());
         const auto QPq = Q(mod_->selPqFromAll());

         const auto IZiprPq = real(mod_->IZip()(mod_->selPqFromAll()));
         const auto IZipiPq = imag(mod_->IZip()(mod_->selPqFromAll()));

         Col<double> M2Pq = VrPq % VrPq + ViPq % ViPq;

         f(selIrPqFrom_f_) = (VrPq % PPq + ViPq % QPq) / M2Pq
            + IZiprPq - GPq * Vr + BPq * Vi;
         f(selIiPqFrom_f_) = (ViPq % PPq - VrPq % QPq) / M2Pq
            + IZipiPq - GPq * Vi - BPq * Vr;
      }

      if (mod_->nPv() > 0)
      {
         // PV busses. Note that these differ in that M2Pv is considered a constant.
         const auto GPv = G_(mod_->selPvFromAll(), mod_->selAllFromAll());
         const auto BPv = B_(mod_->selPvFromAll(), mod_->selAllFromAll());

         const auto VrPv = Vr(mod_->selPvFromAll());
         const auto ViPv = Vi(mod_->selPvFromAll());

         const auto PPv = P(mod_->selPvFromAll());
         const auto QPv = Q(mod_->selPvFromAll());

         const auto IZiprPv = real(mod_->IZip()(mod_->selPvFromAll()));
         const auto IZipiPv = imag(mod_->IZip()(mod_->selPvFromAll()));

         f(selIrPvFrom_f_) = (VrPv % PPv + ViPv % QPv) / M2Pv
            + IZiprPv - GPv * Vr + BPv * Vi;
         f(selIiPvFrom_f_) = (ViPv % PPv - VrPv % QPv) / M2Pv
            + IZipiPv - GPv * Vi - BPv * Vr;
      }
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::updateJ(Jacobian& J, const Jacobian& Jc,
                             const Col<double>& Vr, const Col<double>& Vi,
                             const Col<double>& P, const Col<double>& Q,
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
      for (int i = 0; i < mod_->nPq(); ++i)
      {
         int iPqi = mod_->iPq(i);

         double PVr_p_QVi = P(iPqi) * Vr(iPqi) + Q(iPqi) * Vi(iPqi);
         double PVi_m_QVr = P(iPqi) * Vi(iPqi) - Q(iPqi) * Vr(iPqi);
         double M2 = Vr(iPqi) * Vr(iPqi) + Vi(iPqi) * Vi(iPqi);
         double M4 = M2 * M2;
         double VrdM4 = Vr(iPqi) / M4;
         double VidM4 = Vi(iPqi) / M4;
         double PdM2 = P(iPqi) / M2;
         double QdM2 = Q(iPqi) / M2;

         J.IrPqVrPq()(i, i) = Jc.IrPqVrPq()(i, i) - (2 * VrdM4 * PVr_p_QVi) + PdM2;
         J.IrPqViPq()(i, i) = Jc.IrPqViPq()(i, i) - (2 * VidM4 * PVr_p_QVi) + QdM2;
         J.IiPqVrPq()(i, i) = Jc.IiPqVrPq()(i, i) - (2 * VrdM4 * PVi_m_QVr) - QdM2;
         J.IiPqViPq()(i, i) = Jc.IiPqViPq()(i, i) - (2 * VidM4 * PVi_m_QVr) + PdM2;
      }

      // For PV busses, M^2 is constant, and therefore we can write the Jacobian more simply.
      for (int i = 0; i < mod_->nPv(); ++i)
      {
         int iPvi = mod_->iPv(i);

         J.IrPvVrPv()(i, i) = Jc.IrPvVrPv()(i, i) + P(iPvi) / M2Pv(i); // Could -> Jc if we wanted.
         J.IrPvViPv()(i, i) = Jc.IrPvViPv()(i, i) + Q(iPvi) / M2Pv(i);
         J.IiPvVrPv()(i, i) = Jc.IiPvVrPv()(i, i) - Q(iPvi) / M2Pv(i);
         J.IiPvViPv()(i, i) = Jc.IiPvViPv()(i, i) + P(iPvi) / M2Pv(i);
      }

      if (mod_->nPv() > 0)
      {
         // Set the PV Q columns in the Jacobian. They are diagonal.
         const auto VrPv = Vr(mod_->selPvFromAll());
         const auto ViPv = Vi(mod_->selPvFromAll());
         for (int i = 0; i < mod_->nPv(); ++i)
         {
            J.IrPvQPv()(i, i) = ViPv(i) / M2Pv(i);
            J.IiPvQPv()(i, i) = -VrPv(i) / M2Pv(i);
         }
      }
   }

   // Modify J and f to take into account PV busses.
   void PowerFlowNr::modifyForPv(Jacobian& J, Col<double>& f,
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

      const auto VrPv = Vr(mod_->selPvFromAll());
      const auto ViPv = Vi(mod_->selPvFromAll());

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

   void PowerFlowNr::calcJMatrix(SpMat<double>& JMat, const Jacobian& J) const
   {
      Array<int, 4> ibInd = {{0, 1, 2, 3}};
      Array<int, 4> kbInd = {{0, 1, 3, 4}}; // Skip VrPv, since it doesn't appear as a variable.
      Array<Col<uword>, 4> sl1Vec = {{selIrPqFrom_f_, selIiPqFrom_f_, selIrPvFrom_f_, selIiPvFrom_f_}};
      Array<Col<uword>, 4> sl2Vec = {{selVrPqFrom_x_, selViPqFrom_x_, selViPvFrom_x_, selQPvFrom_x_}};

      JMat = SpMat<double>(nVar(), nVar());

      // Loop over all blocks in J.
      for (int ib = 0; ib < 4; ++ib)
      {
         Col<uword>& sl1 = sl1Vec[ib];
         for (int kb = 0; kb < 4; ++kb)
         {
            Col<uword>& sl2 = sl2Vec[kb];
            const SpMat<double>& block = J.blocks_[ibInd[ib]][kbInd[kb]];

            // Loop over all non-zero elements in the block.
            for (auto it = block.begin(); it != block.end(); ++it)
            {
               // Get the indices into the block.
               int iBlock = it.row();
               int kBlock = it.col();

               // Get the indices into JMat.
               int iRes = sl1(iBlock);
               int kRes = sl2(kBlock);

               // Assign the element.
               JMat(iRes, kRes) = block(iBlock, kBlock);
            }
         }
      }
   }
}
