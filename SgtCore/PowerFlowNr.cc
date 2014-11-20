#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNr.h"
#include "SparseSolver.h"
#include "Stopwatch.h"

namespace SmartGridToolbox
{
   namespace
   {
      void initJcBlock(
            const arma::Mat<double>& G,
            const arma::Mat<double>& B,
            arma::SpMat<double>& Jrr,
            arma::SpMat<double>& Jri,
            arma::SpMat<double>& Jir,
            arma::SpMat<double>& Jii)
      {
         Jrr = -G;
         Jri =  B;
         Jir = -B;
         Jii = -G;
      }
   }

   Jacobian::Jacobian(int nPq, int nPv)
   {
      for (int i = 0; i < 2; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k] = arma::SpMat<double>(nPq, nPq);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k] = arma::SpMat<double>(nPq, nPv);
         }
      }
      for (int i = 2; i < 4; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k] = arma::SpMat<double>(nPv, nPq);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k] = arma::SpMat<double>(nPv, nPv);
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

      arma::Col<double> Vr = real(mod_->V());
      arma::Col<double> Vi = imag(mod_->V());

      arma::Col<double> P = real(mod_->S());
      arma::Col<double> Q = imag(mod_->S());

      arma::Col<double> M2Pv = element_prod(project(Vr, mod_->selPvFromAll()), project(Vr, mod_->selPvFromAll()))
                                 + element_prod(project(Vi, mod_->selPvFromAll()), project(Vi, mod_->selPvFromAll()));

      Jacobian Jc(mod_->nPq(), mod_->nPv()); ///< The part of J that doesn't update at each iteration.
      initJc(Jc);

      arma::Col<double> f(nVar()); ///< Current mismatch function.

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

         err = norm_inf(f);
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
         modifyForPv(J, f, Vr, Vi, M2Pv);
         stopwatch.stop(); durationModifyForPv += stopwatch.seconds();

         // Construct the full Jacobian from J, which contains the block structure.
         stopwatch.reset(); stopwatch.start();
         arma::SpMat<double> JMat; calcJMatrix(JMat, J);
         stopwatch.stop(); durationConstructJMat += stopwatch.seconds();

         SGT_DEBUG
         (
            Log().debug() << "Before kluSolve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
            Log().debug() << "Before kluSolve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
            Log().debug() << "Before kluSolve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl;
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
         arma::Col<double> x;
         bool ok = kluSolve(JMat, -f, x);
         stopwatch.stop(); durationSolve += stopwatch.seconds();

         SGT_DEBUG(Log().debug() << "After kluSolve: ok = " << ok << std::endl);
         SGT_DEBUG(Log().debug() << "After kluSolve: x  = " << std::setprecision(5) << std::setw(9) << x 
               << std::endl);
         if (!ok)
         {
            Log().fatal() << "kluSolve failed." << std::endl;
         }

         stopwatch.reset(); stopwatch.start();
         // Update the current values of V from the solution:
         project(Vr, mod_->selPqFromAll()) += project(x, selVrPqFrom_x_);
         project(Vi, mod_->selPqFromAll()) += project(x, selViPqFrom_x_);

         // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
         auto VrPv = project(Vr, mod_->selPvFromAll());
         auto ViPv = project(Vi, mod_->selPvFromAll());
         const auto DeltaViPv = project(x, selViPvFrom_x_);
         VrPv += element_div(M2Pv - element_prod(VrPv, VrPv) - element_prod(ViPv, ViPv)
                             - 2 * element_prod(ViPv, DeltaViPv), 2 * VrPv);
         ViPv += DeltaViPv;

         // Update Q for PV busses based on the solution.
         project(Q, mod_->selPvFromAll()) += project(x, selQPvFrom_x_);

         SGT_DEBUG(Log().debug() << "Updated Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl);
         SGT_DEBUG(Log().debug() << "Updated Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl);
         SGT_DEBUG(Log().debug() << "Updated M^2 = " << std::setprecision(5) << std::setw(9)
                           << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl);
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
         auto SSl = project(mod_->S(), mod_->selSlFromAll());

         auto VSl = project(mod_->V(), mod_->selSlFromAll());
         auto IZipSl = project(mod_->IZip(), mod_->selSlFromAll());

         auto YStar = conj(project(mod_->Y(), mod_->selSlFromAll(), mod_->selAllFromAll()));
         auto VStar = conj(mod_->V());
         auto IZipStar = conj(project(mod_->IZip(), mod_->selSlFromAll()));

         SSl = element_prod(VSl, prod(YStar, VStar)) - element_prod(VSl, IZipStar);

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
         Log().fatal() << "PowerFlowNr: Newton-Raphson method failed to converge." << std::endl;
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
      for (int i = 0; i < mod_->nPq(); ++i)
      {
         selIrPqFrom_f_[i] = 2 * i + 1;
         selIiPqFrom_f_[i] = 2 * i;
      }

      for (int i = 0; i < mod_->nPv(); ++i)
      {
         selIrPvFrom_f_[i] = 2 * mod_->nPq() + 2 * i + 1;
         selIiPvFrom_f_[i] = 2 * mod_->nPq() + 2 * i;
      }

      for (int i = 0; i < mod_->nPq(); ++i)
      {
         selVrPqFrom_x_[i] = 2 * i;
         selViPqFrom_x_[i] = 2 * i + 1;
      }

      for (int i = 0; i < mod_->nPv(); ++i)
      {
         selQPvFrom_x_[i] = 2 * mod_->nPq() + 2 * i;
         selViPvFrom_x_[i] = 2 * mod_->nPq() + 2 * i + 1;
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   /** At this stage, we are treating J as if all busses were PQ. */
   void PowerFlowNr::initJc(Jacobian& Jc) const
   {
      initJcBlock(project(G_, mod_->selPqFromAll(), mod_->selPqFromAll()),
                  project(B_, mod_->selPqFromAll(), mod_->selPqFromAll()),
                  Jc.IrPqVrPq(),
                  Jc.IrPqViPq(),
                  Jc.IiPqVrPq(),
                  Jc.IiPqViPq());
      initJcBlock(project(G_, mod_->selPqFromAll(), mod_->selPvFromAll()),
                  project(B_, mod_->selPqFromAll(), mod_->selPvFromAll()),
                  Jc.IrPqVrPv(),
                  Jc.IrPqViPv(),
                  Jc.IiPqVrPv(),
                  Jc.IiPqViPv());
      initJcBlock(project(G_, mod_->selPvFromAll(), mod_->selPqFromAll()),
                  project(B_, mod_->selPvFromAll(), mod_->selPqFromAll()),
                  Jc.IrPvVrPq(),
                  Jc.IrPvViPq(),
                  Jc.IiPvVrPq(),
                  Jc.IiPvViPq());
      initJcBlock(project(G_, mod_->selPvFromAll(), mod_->selPvFromAll()),
                  project(B_, mod_->selPvFromAll(), mod_->selPvFromAll()),
                  Jc.IrPvVrPv(),
                  Jc.IrPvViPv(),
                  Jc.IiPvVrPv(),
                  Jc.IiPvViPv());
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::calcf(arma::Col<double>& f,
                           const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                           const arma::Col<double>& P, const arma::Col<double>& Q,
                           const arma::Col<double>& M2Pv) const
   {
      // PQ busses:
      const arma::SpMat<double> GPq = project(G_, mod_->selPqFromAll(), mod_->selAllFromAll());
      const arma::SpMat<double> BPq = project(B_, mod_->selPqFromAll(), mod_->selAllFromAll());

      const auto VrPq = project(Vr, mod_->selPqFromAll());
      const auto ViPq = project(Vi, mod_->selPqFromAll());

      const auto PPq = project(P, mod_->selPqFromAll());
      const auto QPq = project(Q, mod_->selPqFromAll());

      const auto IZiprPq = project(real(mod_->IZip()), mod_->selPqFromAll());
      const auto IZipiPq = project(imag(mod_->IZip()), mod_->selPqFromAll());

      arma::Col<double> M2Pq = element_prod(VrPq, VrPq) + element_prod(ViPq, ViPq);

      project(f, selIrPqFrom_f_) = element_div(element_prod(VrPq, PPq) + element_prod(ViPq, QPq), M2Pq)
                                 + IZiprPq - myProd(GPq, Vr) + myProd(BPq, Vi);
      project(f, selIiPqFrom_f_) = element_div(element_prod(ViPq, PPq) - element_prod(VrPq, QPq), M2Pq)
                                 + IZipiPq - myProd(GPq, Vi) - myProd(BPq, Vr);

      // PV busses. Note that these differ in that M2Pv is considered a constant.
      const auto GPv = project(G_, mod_->selPvFromAll(), mod_->selAllFromAll());
      const auto BPv = project(B_, mod_->selPvFromAll(), mod_->selAllFromAll());

      const auto VrPv = project(Vr, mod_->selPvFromAll());
      const auto ViPv = project(Vi, mod_->selPvFromAll());

      const auto PPv = project(P, mod_->selPvFromAll());
      const auto QPv = project(Q, mod_->selPvFromAll());

      const auto IZiprPv = project(real(mod_->IZip()), mod_->selPvFromAll());
      const auto IZipiPv = project(imag(mod_->IZip()), mod_->selPvFromAll());

      project(f, selIrPvFrom_f_) = element_div(element_prod(VrPv, PPv) + element_prod(ViPv, QPv), M2Pv)
                                 + IZiprPv - myProd(GPv, Vr) + myProd(BPv, Vi);
      project(f, selIiPvFrom_f_) = element_div(element_prod(ViPv, PPv) - element_prod(VrPv, QPv), M2Pv)
                                 + IZipiPv - myProd(GPv, Vi) - myProd(BPv, Vr);
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::updateJ(Jacobian& J, const Jacobian& Jc,
                             const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                             const arma::Col<double>& P, const ublas::vector <double>& Q,
                             const arma::Col<double>& M2Pv) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in Jc at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.

      // Reset PV Vi columns, since these get messed with:
      J.IrPqViPv() = Jc.IrPqViPv();
      J.IiPqViPv() = Jc.IiPqViPv();
      J.IrPvViPv() = Jc.IrPvViPv();
      J.IiPvViPv() = Jc.IiPvViPv();

      // Block diagonal:
      for (int i = 0; i < mod_->nPq(); ++i)
      {
         int iPqi = mod_->iPq(i);

         double Pvr_p_QVi = P(iPqi) * Vr(iPqi) + Q(iPqi) * Vi(iPqi);
         double Pvi_m_QVr = P(iPqi) * Vi(iPqi) - Q(iPqi) * Vr(iPqi);
         double M2 = Vr(iPqi) * Vr(iPqi) + Vi(iPqi) * Vi(iPqi);
         double M4 = M2 * M2;
         double VrdM4 = Vr(iPqi) / M4;
         double VidM4 = Vi(iPqi) / M4;
         double PdM2 = P(iPqi) / M2;
         double QdM2 = Q(iPqi) / M2;

         J.IrPqVrPq()(i, i) = Jc.IrPqVrPq()(i, i) - (2 * VrdM4 * Pvr_p_QVi) + PdM2;
         J.IrPqViPq()(i, i) = Jc.IrPqViPq()(i, i) - (2 * VidM4 * Pvr_p_QVi) + QdM2;
         J.IiPqVrPq()(i, i) = Jc.IiPqVrPq()(i, i) - (2 * VrdM4 * Pvi_m_QVr) - QdM2;
         J.IiPqViPq()(i, i) = Jc.IiPqViPq()(i, i) - (2 * VidM4 * Pvi_m_QVr) + PdM2;
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

      // Set the PV Q columns in the Jacobian. They are diagonal.
      const auto VrPv = project(Vr, mod_->selPvFromAll());
      const auto ViPv = project(Vi, mod_->selPvFromAll());
      for (int i = 0; i < mod_->nPv(); ++i)
      {
         J.IrPvQPv()(i, i) = ViPv(i) / M2Pv(i);
         J.IiPvQPv()(i, i) = -VrPv(i) / M2Pv(i);
      }
   }

   // Modify J and f to take into account PV busses.
   void PowerFlowNr::modifyForPv(Jacobian& J, arma::Col<double>& f,
                                 const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                                 const arma::Col<double>& M2Pv)
   {
      const auto VrPv = project(Vr, mod_->selPvFromAll());
      const auto ViPv = project(Vi, mod_->selPvFromAll());

      typedef ublas::vector_slice<arma::Col<double>> VecSel;
      typedef ublas::matrix_column<arma::SpMat<double>> Column;
      auto mod = [](VecSel fProj, Column colViPv, const Column colVrPv, double fMult, double colViPvMult)
      {
         for (auto it = colVrPv.begin(); it != colVrPv.end(); ++it)
         {
            int idx = it.index();
            fProj(idx) += colVrPv(idx) * fMult;
            colViPv(idx) += colVrPv(idx) * colViPvMult;
         }
      };

      for (int k = 0; k < mod_->nPv(); ++k)
      {
         double fMult = (0.5 * (M2Pv(k) - VrPv(k) * VrPv(k) - ViPv(k) * ViPv(k)) / VrPv(k));
         double colViPvMult = -ViPv(k) / VrPv(k);

         mod(project(f, selIrPqFrom_f_), column(J.IrPqViPv(), k), column(J.IrPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPqFrom_f_), column(J.IiPqViPv(), k), column(J.IiPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIrPvFrom_f_), column(J.IrPvViPv(), k), column(J.IrPvVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPvFrom_f_), column(J.IiPvViPv(), k), column(J.IiPvVrPv(), k), fMult, colViPvMult);
      }
   }

   void PowerFlowNr::calcJMatrix(arma::SpMat<double>& JMat, const Jacobian& J) const
   {
      Array<int, 4> ibInd = {{0, 1, 2, 3}};
      Array<int, 4> kbInd = {{0, 1, 3, 4}}; // Skip VrPv, since it doesn't appear as a variable.
      Array<ublas::slice, 4> sl1Vec = {{selIrPqFrom_f_, selIiPqFrom_f_, selIrPvFrom_f_, selIiPvFrom_f_}};
      Array<ublas::slice, 4> sl2Vec = {{selVrPqFrom_x_, selViPqFrom_x_, selViPvFrom_x_, selQPvFrom_x_}};

      JMat = arma::SpMat<double>(nVar(), nVar());

      // Loop over all blocks in J.
      for (int ib = 0; ib < 4; ++ib)
      {
         ublas::slice sl1 = sl1Vec[ib];
         for (int kb = 0; kb < 4; ++kb)
         {
            ublas::slice sl2 = sl2Vec[kb];
            const arma::SpMat<double>& block = J.blocks_[ibInd[ib]][kbInd[kb]];

            // Loop over all non-zero elements in the block.
            for (auto it1 = block.begin1(); it1 != block.end1(); ++it1)
            {
               for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
               {
                  // Get the indices into the block.
                  int iBlock = it2.index1();
                  int kBlock = it2.index2();

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
}
