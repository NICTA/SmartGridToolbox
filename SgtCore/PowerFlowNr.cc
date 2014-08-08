#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNr.h"
#include <SgtSim/SparseSolver.h>
#include <SgtSim/Stopwatch.h>

namespace SmartGridToolbox
{
   namespace
   {
      // For some reason, even ublas::axpy_prod is much slower than this!
      ublas::vector<double> myProd(const ublas::compressed_matrix<double>& A, const ublas::vector<double>& x)
      {
         ublas::vector<double> result(A.size1(), 0.0);
         for (auto it1 = A.begin1(); it1 != A.end1(); ++it1)
         {
            for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
            {
               int i = it2.index1();
               int k = it2.index2();
               result(i) += A(i, k)*x(k);
            }
         }
         return result;
      }

      void initJcBlock(
            const ublas::matrix_range<const ublas::compressed_matrix<double>>& G,
            const ublas::matrix_range<const ublas::compressed_matrix<double>>& B,
            ublas::compressed_matrix<double>& Jrr,
            ublas::compressed_matrix<double>& Jri,
            ublas::compressed_matrix<double>& Jir,
            ublas::compressed_matrix<double>& Jii)
      {
         Jrr = -G;
         Jri =  B;
         Jir = -B;
         Jii = -G;
      }
   }

   BusNr::BusNr(const std::string& id, BusType type, Phases phases, const ublas::vector<Complex>& V,
         const ublas::vector<Complex>& Ys, const ublas::vector<Complex>& Ic,
         const ublas::vector<Complex>& S) :
      id_(id),
      type_(type),
      phases_(phases),
      V_(V),
      S_(S),
      Ys_(Ys),
      Ic_(Ic)
   {
      assert(V.size() == phases.size());
      assert(Ys.size() == phases.size());
      assert(Ic.size() == phases.size());
      assert(S.size() == phases.size());

      for (int i = 0; i < phases.size(); ++i)
      {
         nodes_.push_back(std::unique_ptr<NodeNr>(new NodeNr(*this, i)));
      }
   }

   NodeNr::NodeNr(BusNr& bus, int phaseIdx) :
      bus_(&bus),
      phaseIdx_(phaseIdx),
      V_(bus.V_(phaseIdx)),
      S_(bus.S_(phaseIdx)),
      Ys_(bus.Ys_(phaseIdx)),
      Ic_(bus.Ic_(phaseIdx)),
      idx_(-1)
   {
      // Empty.
   }

   BranchNr::BranchNr(const std::string& id0, const std::string& id1, Phases phases0, Phases phases1,
                      const ublas::matrix<Complex>& Y) :
      nPhase_(phases0.size()),
      ids_{{id0, id1}},
      phases_{{phases0, phases1}},
      Y_(Y)
   {
      assert(phases1.size() == nPhase_);
      int nTerm = 2*nPhase_;
      assert(Y.size1() == nTerm);
      assert(Y.size2() == nTerm);
   }

   Jacobian::Jacobian(int nPq, int nPv)
   {
      for (int i = 0; i < 2; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPq, nPq, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPq, nPv, false);
         }
      }
      for (int i = 2; i < 4; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPv, nPq, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPv, nPv, false);
         }
      }
   }

   void PowerFlowNr::addBus(const std::string& id, BusType type, Phases phases, const ublas::vector<Complex>& V,
         const ublas::vector<Complex>& Y, const ublas::vector<Complex>& I, const ublas::vector<Complex>& S)
   {
      SGT_DEBUG(debug() << "PowerFlowNr : add bus " << id << std::endl);
      busses_[id].reset(new BusNr(id, type, phases, V, Y, I, S));
   }

   void PowerFlowNr::addBranch(const std::string& idBus0, const std::string& idBus1, Phases phases0, Phases phases1,
                               const ublas::matrix<Complex>& Y)
   {
      SGT_DEBUG(debug() << "PowerFlowNr : addBranch " << idBus0 << " " << idBus1 << std::endl);
      branches_.push_back(std::unique_ptr<BranchNr>(new BranchNr(idBus0, idBus1, phases0, phases1, Y)));
   }

   void PowerFlowNr::reset()
   {
      SGT_DEBUG(debug() << "PowerFlowNr : reset." << std::endl);
      busses_ = BusMap();
      branches_ = BranchVec();
   }

   void PowerFlowNr::validate()
   {
      SGT_DEBUG(debug() << "PowerFlowNr : validate." << std::endl);

      // Make Nodes:
      NodeVec PqNodes = NodeVec();
      NodeVec PvNodes = NodeVec();
      NodeVec SlNodes = NodeVec();
      for (auto& busPair : busses_)
      {
         BusNr& bus = *busPair.second;
         NodeVec* vec = nullptr;
         if (bus.type_ == BusType::PQ)
         {
            vec = &PqNodes;
         }
         else if (bus.type_ == BusType::PV)
         {
            vec = &PvNodes;
         }
         else if (bus.type_ == BusType::SL)
         {
            vec = &SlNodes;
         }
         else
         {
            error() << "Unsupported bus type " << busType2Str(bus.type_) << std::endl;
            abort();
         }
         for (const std::unique_ptr<NodeNr>& node : bus.nodes_)
         {
            vec->push_back(node.get());
         }
      }

      // Determine sizes:
      nPq_ = PqNodes.size();
      nPv_ = PvNodes.size();
      nSl_ = SlNodes.size();
      assert(nSl_ > 0); // TODO: What is correct here?

      // Insert nodes into ordered list of all nodes. Be careful of ordering!
      nodes_ = NodeVec();
      nodes_.reserve(nNode());
      nodes_.insert(nodes_.end(), SlNodes.begin(), SlNodes.end());
      nodes_.insert(nodes_.end(), PqNodes.begin(), PqNodes.end());
      nodes_.insert(nodes_.end(), PvNodes.begin(), PvNodes.end());
      // Index all nodes:
      for (int i = 0; i < nNode(); ++i)
      {
         nodes_[i]->idx_ = i;
      }

      Y_.resize(nNode(), nNode(), false);

      // Branch admittances:
      for (const std::unique_ptr<BranchNr>& branch : branches_)
      {
         auto it0 = busses_.find(branch->ids_[0]);
         if (it0 == busses_.end())
         {
            error() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1] << " contains a non-existent bus "
                    << branch->ids_[0] << std::endl;
            abort();
         }
         auto it1 = busses_.find(branch->ids_[1]);
         if (it1 == busses_.end())
         {
            error() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1] << " contains a non-existent bus "
                    << branch->ids_[1] << std::endl;
            abort();
         }
         const BusNr* busses[] = {it0->second.get(), it1->second.get()};
         int nTerm = 2*branch->nPhase_;

         // There is one link per distinct pair of bus/phase pairs.
         for (int i = 0; i < nTerm; ++i)
         {
            int busIdxI = i/branch->nPhase_; // 0 or 1
            int branchPhaseIdxI = i % branch->nPhase_; // 0 to nPhase of branch.
            const BusNr* busI = busses[busIdxI];
            int busPhaseIdxI = busI->phases_.phaseIndex(branch->phases_[busIdxI][branchPhaseIdxI]);
            const NodeNr* nodeI = busI->nodes_[busPhaseIdxI].get();
            int idxNodeI = nodeI->idx_;

            // Only count each diagonal element in branch->Y_ once!
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i);

            for (int k = i + 1; k < nTerm; ++k)
            {
               int busIdxK = k/branch->nPhase_; // 0 or 1
               int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
               const BusNr* busK = busses[busIdxK];
               int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
               const NodeNr* nodeK = busK->nodes_[busPhaseIdxK].get();
               int idxNodeK = nodeK->idx_;

               Y_(idxNodeI, idxNodeK) += branch->Y_(i, k);
               Y_(idxNodeK, idxNodeI) += branch->Y_(k, i);
            }
         }
      } // Loop over branches.

      // Add shunt terms:
      for (int i = 0; i < nNode(); ++i)
      {
         Y_(i, i) += nodes_[i]->Ys_;
      }

      G_ = real(Y_);
      B_ = imag(Y_);
      SGT_DEBUG(debug() << "Y_.nnz() = " << Y_.nnz() << std::endl);

      // Load quantities.
      Ic_.resize(nNode(), false);
      for (int i = 0; i < nNode(); ++i)
      {
         Ic_(i) = nodes_[i]->Ic_;
      }

      SGT_DEBUG(debug() << "PowerFlowNr : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   /// Initialize voltages:
   void PowerFlowNr::initV(ublas::vector<double>& Vr, ublas::vector<double>& Vi) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNr& node = *nodes_[i];
         Vr(i) = node.V_.real();
         Vi(i) = node.V_.imag();
      }
   }

   void PowerFlowNr::initS(ublas::vector<double>& P, ublas::vector<double>& Q) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNr& node = *nodes_[i];
         P(i) = node.S_.real();
         Q(i) = node.S_.imag();
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   /** At this stage, we are treating J as if all busses were PQ. */
   void PowerFlowNr::initJc(Jacobian& Jc) const
   {
      initJcBlock(project(G_, selPqFromAll(), selPqFromAll()),
                  project(B_, selPqFromAll(), selPqFromAll()),
                  Jc.IrPqVrPq(),
                  Jc.IrPqViPq(),
                  Jc.IiPqVrPq(),
                  Jc.IiPqViPq());
      initJcBlock(project(G_, selPqFromAll(), selPvFromAll()),
                  project(B_, selPqFromAll(), selPvFromAll()),
                  Jc.IrPqVrPv(),
                  Jc.IrPqViPv(),
                  Jc.IiPqVrPv(),
                  Jc.IiPqViPv());
      initJcBlock(project(G_, selPvFromAll(), selPqFromAll()),
                  project(B_, selPvFromAll(), selPqFromAll()),
                  Jc.IrPvVrPq(),
                  Jc.IrPvViPq(),
                  Jc.IiPvVrPq(),
                  Jc.IiPvViPq());
      initJcBlock(project(G_, selPvFromAll(), selPvFromAll()),
                  project(B_, selPvFromAll(), selPvFromAll()),
                  Jc.IrPvVrPv(),
                  Jc.IrPvViPv(),
                  Jc.IiPvVrPv(),
                  Jc.IiPvViPv());
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::calcf(ublas::vector<double>& f,
                           const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                           const ublas::vector<double>& P, const ublas::vector<double>& Q,
                           const ublas::vector<double>& M2Pv) const
   {
      // PQ busses:
      const ublas::compressed_matrix<double> GPq = project(G_, selPqFromAll(), selAllFromAll());
      const ublas::compressed_matrix<double> BPq = project(B_, selPqFromAll(), selAllFromAll());

      const auto VrPq = project(Vr, selPqFromAll());
      const auto ViPq = project(Vi, selPqFromAll());

      const auto PPq = project(P, selPqFromAll());
      const auto QPq = project(Q, selPqFromAll());

      const auto IcrPq = project(real(Ic_), selPqFromAll());
      const auto IciPq = project(imag(Ic_), selPqFromAll());

      ublas::vector<double> M2Pq = element_prod(VrPq, VrPq) + element_prod(ViPq, ViPq);

      project(f, selIrPqFrom_f()) = element_div(element_prod(VrPq, PPq) + element_prod(ViPq, QPq), M2Pq)
                                 + IcrPq - myProd(GPq, Vr) + myProd(BPq, Vi);
      project(f, selIiPqFrom_f()) = element_div(element_prod(ViPq, PPq) - element_prod(VrPq, QPq), M2Pq)
                                 + IciPq - myProd(GPq, Vi) - myProd(BPq, Vr);

      // PV busses. Note that these differ in that M2Pv is considered a constant.
      const auto GPv = project(G_, selPvFromAll(), selAllFromAll());
      const auto BPv = project(B_, selPvFromAll(), selAllFromAll());

      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());

      const auto PPv = project(P, selPvFromAll());
      const auto QPv = project(Q, selPvFromAll());

      const auto IcrPv = project(real(Ic_), selPvFromAll());
      const auto IciPv = project(imag(Ic_), selPvFromAll());

      project(f, selIrPvFrom_f()) = element_div(element_prod(VrPv, PPv) + element_prod(ViPv, QPv), M2Pv)
                                 + IcrPv - myProd(GPv, Vr) + myProd(BPv, Vi);
      project(f, selIiPvFrom_f()) = element_div(element_prod(ViPv, PPv) - element_prod(VrPv, QPv), M2Pv)
                                 + IciPv - myProd(GPv, Vi) - myProd(BPv, Vr);
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::updateJ(Jacobian& J, const Jacobian& Jc,
                             const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                             const ublas::vector<double>& P, const ublas::vector <double>& Q,
                             const ublas::vector<double>& M2Pv) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in Jc at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.

      // Reset PV Vi columns, since these get messed with:
      J.IrPqViPv() = Jc.IrPqViPv();
      J.IiPqViPv() = Jc.IiPqViPv();
      J.IrPvViPv() = Jc.IrPvViPv();
      J.IiPvViPv() = Jc.IiPvViPv();

      // Block diagonal:
      for (int i = 0; i < nPq_; ++i)
      {
         int iPqi = iPq(i);

         double Pvr_p_QVi = P(iPqi)*Vr(iPqi) + Q(iPqi)*Vi(iPqi);
         double Pvi_m_QVr = P(iPqi)*Vi(iPqi) - Q(iPqi)*Vr(iPqi);
         double M2 = Vr(iPqi)*Vr(iPqi) + Vi(iPqi)*Vi(iPqi);
         double M4 = M2*M2;
         double VrdM4 = Vr(iPqi)/M4;
         double VidM4 = Vi(iPqi)/M4;
         double PdM2 = P(iPqi)/M2;
         double QdM2 = Q(iPqi)/M2;

         J.IrPqVrPq()(i, i) = Jc.IrPqVrPq()(i, i) - (2*VrdM4*Pvr_p_QVi) + PdM2;
         J.IrPqViPq()(i, i) = Jc.IrPqViPq()(i, i) - (2*VidM4*Pvr_p_QVi) + QdM2;
         J.IiPqVrPq()(i, i) = Jc.IiPqVrPq()(i, i) - (2*VrdM4*Pvi_m_QVr) - QdM2;
         J.IiPqViPq()(i, i) = Jc.IiPqViPq()(i, i) - (2*VidM4*Pvi_m_QVr) + PdM2;
      }

      // For PV busses, M^2 is constant, and therefore we can write the Jacobian more simply.
      for (int i = 0; i < nPv_; ++i)
      {
         int iPvi = iPv(i);

         J.IrPvVrPv()(i, i) = Jc.IrPvVrPv()(i, i) + P(iPvi)/M2Pv(i); // Could -> Jc if we wanted.
         J.IrPvViPv()(i, i) = Jc.IrPvViPv()(i, i) + Q(iPvi)/M2Pv(i);
         J.IiPvVrPv()(i, i) = Jc.IiPvVrPv()(i, i) - Q(iPvi)/M2Pv(i);
         J.IiPvViPv()(i, i) = Jc.IiPvViPv()(i, i) + P(iPvi)/M2Pv(i);
      }

      // Set the PV Q columns in the Jacobian. They are diagonal.
      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());
      for (int i = 0; i < nPv_; ++i)
      {
         J.IrPvQPv()(i, i) = ViPv(i)/M2Pv(i);
         J.IiPvQPv()(i, i) = -VrPv(i)/M2Pv(i);
      }
   }

   // Modify J and f to take into account PV busses.
   void PowerFlowNr::modifyForPv(Jacobian& J, ublas::vector<double>& f,
                                 const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                                 const ublas::vector<double>& M2Pv)
   {
      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());

      typedef ublas::vector_slice<ublas::vector<double>> VecSel;
      typedef ublas::matrix_column<ublas::compressed_matrix<double>> Column;
      auto mod = [](VecSel fProj, Column colViPv, const Column colVrPv, double fMult, double colViPvMult)
      {
         for (auto it = colVrPv.begin(); it != colVrPv.end(); ++it)
         {
            int idx = it.index();
            fProj(idx) += colVrPv(idx)*fMult;
            colViPv(idx) += colVrPv(idx)*colViPvMult;
         }
      };

      for (int k = 0; k < nPv_; ++k)
      {
         double fMult = (0.5*(M2Pv(k) - VrPv(k)*VrPv(k) - ViPv(k)*ViPv(k))/VrPv(k));
         double colViPvMult = -ViPv(k)/VrPv(k);

         mod(project(f, selIrPqFrom_f()), column(J.IrPqViPv(), k), column(J.IrPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPqFrom_f()), column(J.IiPqViPv(), k), column(J.IiPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIrPvFrom_f()), column(J.IrPvViPv(), k), column(J.IrPvVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPvFrom_f()), column(J.IiPvViPv(), k), column(J.IiPvVrPv(), k), fMult, colViPvMult);
      }
   }

   void PowerFlowNr::calcJMatrix(ublas::compressed_matrix<double>& JMat, const Jacobian& J) const
   {
      Array<int, 4> ibInd = {0, 1, 2, 3};
      Array<int, 4> kbInd = {0, 1, 3, 4}; // Skip VrPv, since it doesn't appear as a variable.
      Array<ublas::slice, 4> sl1Vec = {selIrPqFrom_f(), selIiPqFrom_f(), selIrPvFrom_f(), selIiPvFrom_f()};
      Array<ublas::slice, 4> sl2Vec = {selVrPqFrom_x(), selViPqFrom_x(), selViPvFrom_x(), selQPvFrom_x()};

      JMat = ublas::compressed_matrix<double>(nVar(), nVar());

      // Loop over all blocks in J.
      for (int ib = 0; ib < 4; ++ib)
      {
         ublas::slice sl1 = sl1Vec[ib];
         for (int kb = 0; kb < 4; ++kb)
         {
            ublas::slice sl2 = sl2Vec[kb];
            const ublas::compressed_matrix<double>& block = J.blocks_[ibInd[ib]][kbInd[kb]];

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

   bool PowerFlowNr::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNr : solve." << std::endl);

      Stopwatch stopwatch;
      Stopwatch stopwatchTot;

      double durationInitSetup;
      double durationCalcf;
      double durationUpdateJ;
      double durationModifyForPv;
      double durationConstructJMat;
      double durationSolve;
      double durationUpdateIter;
      double durationTot;

      stopwatchTot.reset(); stopwatchTot.start();

      // Do the initial setup.
      stopwatch.reset(); stopwatch.start();

      const double tol = 1e-8;
      const int maxiter = 20;

      ublas::vector<double> Vr(nNode());
      ublas::vector<double> Vi(nNode());
      initV(Vr, Vi);

      ublas::vector<double> M2Pv = element_prod(project(Vr, selPvFromAll()), project(Vr, selPvFromAll()))
                                 + element_prod(project(Vi, selPvFromAll()), project(Vi, selPvFromAll()));

      ublas::vector<double> P(nNode());
      ublas::vector<double> Q(nNode());
      initS(P, Q);

      Jacobian Jc(nPq_, nPv_); ///< The part of J that doesn't update at each iteration.
      initJc(Jc);

      ublas::vector<double> f(nVar()); ///< Current mismatch function.

      Jacobian J = Jc; ///< Jacobian, d f_i/d x_i.

      bool wasSuccessful = false;
      double err = 0;
      int niter;

      stopwatch.stop(); durationInitSetup = stopwatch.seconds();

      for (niter = 0; niter < maxiter; ++niter)
      {
         SGT_DEBUG(debug() << "\tIteration = " << niter << std::endl);

         stopwatch.reset(); stopwatch.start();
         calcf(f, Vr, Vi, P, Q, M2Pv);

         err = norm_inf(f);
         SGT_DEBUG(debug() << "\tf  = " << std::setprecision(5) << std::setw(9) << f << std::endl);
         SGT_DEBUG(debug() << "\tError = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(debug() << "\tSuccess at iteration " << niter << "." << std::endl);
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
         ublas::compressed_matrix<double> JMat; calcJMatrix(JMat, J);
         stopwatch.stop(); durationConstructJMat += stopwatch.seconds();

         SGT_DEBUG
         (
            debug() << "\tBefore kluSolve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
            debug() << "\tBefore kluSolve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
            debug() << "\tBefore kluSolve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl;
            debug() << "\tBefore kluSolve: P   = " << std::setprecision(5) << std::setw(9) << P << std::endl;
            debug() << "\tBefore kluSolve: Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl;
            debug() << "\tBefore kluSolve: f   = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            debug() << "\tBefore kluSolve: J   = " << std::endl;
            for (int i = 0; i < nVar(); ++i)
            {
               debug() << "\t\t" << std::setprecision(5) << std::setw(9) << row(JMat, i) << std::endl;
            }
         );

         stopwatch.reset(); stopwatch.start();
         ublas::vector<double> x;
         bool ok = kluSolve(JMat, -f, x);
         stopwatch.stop(); durationSolve += stopwatch.seconds();

         SGT_DEBUG(debug() << "\tAfter kluSolve: ok = " << ok << std::endl);
         SGT_DEBUG(debug() << "\tAfter kluSolve: x  = " << std::setprecision(5) << std::setw(9) << x << std::endl);
         if (!ok)
         {
            error() << "kluSolve failed." << std::endl;
            abort();
         }

         stopwatch.reset(); stopwatch.start();
         // Update the current values of V from the solution:
         project(Vr, selPqFromAll()) += project(x, selVrPqFrom_x());
         project(Vi, selPqFromAll()) += project(x, selViPqFrom_x());

         // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
         auto VrPv = project(Vr, selPvFromAll());
         auto ViPv = project(Vi, selPvFromAll());
         const auto DeltaViPv = project(x, selViPvFrom_x());
         VrPv += element_div(M2Pv - element_prod(VrPv, VrPv) - element_prod(ViPv, ViPv)
                             - 2*element_prod(ViPv, DeltaViPv), 2*VrPv);
         ViPv += DeltaViPv;

         // Update Q for PV busses based on the solution.
         project(Q, selPvFromAll()) += project(x, selQPvFrom_x());

         SGT_DEBUG(debug() << "\tUpdated Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl);
         SGT_DEBUG(debug() << "\tUpdated Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl);
         SGT_DEBUG(debug() << "\tUpdated M^2 = " << std::setprecision(5) << std::setw(9)
                           << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl);
         SGT_DEBUG(debug() << "\tUpdated P   = " << std::setprecision(5) << std::setw(9) << P << std::endl);
         SGT_DEBUG(debug() << "\tUpdated Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl);
         stopwatch.stop(); durationUpdateIter += stopwatch.seconds();
      }

      if (wasSuccessful)
      {
         ublas::vector<Complex> V(nNode());
         ublas::vector<Complex> S(nNode());

         for (int i = 0; i < nNode(); ++i)
         {
            V(i) = {Vr(i), Vi(i)};
            S(i) = {P(i), Q(i)};
         }

         // Set the slack power.
         auto SSl = project(S, selSlFromAll());

         auto VSl = project(V, selSlFromAll());
         auto IcSl = project(Ic_, selSlFromAll());

         auto YStar = conj(project(Y_, selSlFromAll(), selAllFromAll()));
         auto VStar = conj(V);
         auto IcStar = conj(project(Ic_, selSlFromAll()));

         SSl = element_prod(VSl, prod(YStar, VStar)) - element_prod(VSl, IcStar);

         // Update nodes and busses.
         for (int i = 0; i < nNode(); ++i)
         {
            NodeNr* node = nodes_[i];
            node->V_ = V(i);
            node->S_ = S(i);
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
         }
      }
      else
      {
         warning() << "PowerFlowNr: Newton-Raphson method failed to converge." << std::endl;
      }

      stopwatchTot.stop(); durationTot = stopwatchTot.seconds();

      message() << "PowerFlowNr: successful = " << wasSuccessful << ", error = " << err
                << ", iterations = " << niter << ", total time = " << durationTot << "." << std::endl;
      SGT_DEBUG(debug() << "PowerFlowNr: -----------------------   " << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: init setup time         = " << durationInitSetup << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: time in calcf           = " << durationCalcf << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: time in updateJ         = " << durationUpdateJ << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: time in modifyForPv     = " << durationModifyForPv << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: time to construct JMat  = " << durationConstructJMat << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: solve time              = " << durationSolve << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: time to update iter     = " << durationUpdateIter << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNr: -----------------------   " << std::endl);

      return wasSuccessful;
   }

   void PowerFlowNr::printProblem()
   {
      debug() << "PowerFlowNr::printProblem()" << std::endl;
      debug() << "\tNodes:" << std::endl;
      for (const NodeNr* nd : nodes_)
      {
         debug() << "\t\tNode:" << std::endl;
         debug() << "\t\t\tId    : " << nd->bus_->id_ << std::endl;
         debug() << "\t\t\tType  : " << nd->bus_->type_ << std::endl;
         debug() << "\t\t\tPhase : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
         debug() << "\t\t\tV     : " << nd->V_ << std::endl;
         debug() << "\t\t\tS     : " << nd->S_ << std::endl;
         debug() << "\t\t\tYs    : " << nd->Ys_ << std::endl;
         debug() << "\t\t\tIc    : " << nd->Ic_ << std::endl;
      }
      debug() << "\tBranches:" << std::endl;
      for (const std::unique_ptr<BranchNr>& branch : branches_)
      {
         debug() << "\t\tBranch:" << std::endl;
         debug() << "\t\t\tBusses : " << branch->ids_[0] << ", " << branch->ids_[1] << std::endl;
         debug() << "\t\t\tPhases : " << branch->phases_[0] << ", " << branch->phases_[1] << std::endl;
         debug() << "\t\t\tY      :" << std::endl;
         for (int i = 0; i < branch->Y_.size1(); ++i)
         {
            debug() << "\t\t\t\t" << std::setprecision(14) << std::setw(18) << row(branch->Y_, i) << std::endl;
         }
      }
      debug() << "\tY:" << std::endl;
      for (int i = 0; i < Y_.size1(); ++i)
      {
         debug() << "\t\t\t\t" << std::setprecision(14) << std::setw(18) << row(Y_, i) << std::endl;
      }
   }
}
