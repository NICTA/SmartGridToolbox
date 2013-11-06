#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include <SmartGridToolbox/SparseSolver.h>
#include <SmartGridToolbox/Stopwatch.h>

namespace SmartGridToolbox
{
   // For some reason, even ublas::axpy_prod is much slower than this!
   static ublas::vector<double> myProd(const ublas::compressed_matrix<double> & A, const ublas::vector<double> & x)
   {
      ublas::vector<double> result(A.size1(), 0.0);
      for (auto it1 = A.begin1(); it1 != A.end1(); ++it1)
      {
         for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
         {
            int i = it2.index1();
            int k = it2.index2();
            result(i) += A(i, k) * x(k);
         }
      }
      return result;
   }

   BusNR::BusNR(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
                const ublas::vector<Complex> & ys, const ublas::vector<Complex> & Ic,
                const ublas::vector<Complex> & S) :
      id_(id),
      type_(type),
      phases_(phases),
      V_(V),
      S_(S),
      ys_(ys),
      Ic_(Ic)
   {
      assert(V.size() == phases.size());
      assert(ys.size() == phases.size());
      assert(Ic.size() == phases.size());
      assert(S.size() == phases.size());

      for (int i = 0; i < phases.size(); ++i)
      {
         nodes_.push_back(new NodeNR(*this, i));
      }
   }

   BusNR::~BusNR()
   {
      for (auto node : nodes_) delete node;
   }

   NodeNR::NodeNR(BusNR & bus, int phaseIdx) :
      bus_(&bus),
      phaseIdx_(phaseIdx),
      V_(bus.V_(phaseIdx)),
      S_(bus.S_(phaseIdx)),
      ys_(bus.ys_(phaseIdx)),
      Ic_(bus.Ic_(phaseIdx)),
      idx_(-1)
   {
      // Empty.
   }

   BranchNR::BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1,
                      const ublas::matrix<Complex> & Y) :
      nPhase_(phases0.size()),
      ids_{{id0, id1}},
      phases_{{phases0, phases1}},
      Y_(Y)
   {
      assert(phases1.size() == nPhase_);
      int nTerm = 2 * nPhase_;
      assert(Y.size1() == nTerm);
      assert(Y.size2() == nTerm);
   }

   Jacobian::Jacobian(int nPQ, int nPV)
   {
      for (int i = 0; i < 2; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPQ, nPQ, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPQ, nPV, false);
         }
      }
      for (int i = 2; i < 4; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPV, nPQ, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPV, nPV, false);
         }
      }
   }

   PowerFlowNR::~PowerFlowNR()
   {
      for (auto pair : busses_) delete pair.second;
      for (auto branch : branches_) delete branch;
   }

   void PowerFlowNR::addBus(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
         const ublas::vector<Complex> & Y, const ublas::vector<Complex> & I, const ublas::vector<Complex> & S)
   {
      SGT_DEBUG(debug() << "PowerFlowNR : add bus " << id << std::endl);
      busses_[id] = new BusNR(id, type, phases, V, Y, I, S);
   }

   void PowerFlowNR::addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                               const ublas::matrix<Complex> & Y)
   {
      SGT_DEBUG(debug() << "PowerFlowNR : addBranch " << idBus0 << " " << idBus1 << std::endl);
      branches_.push_back(new BranchNR(idBus0, idBus1, phases0, phases1, Y));
   }

   void PowerFlowNR::reset()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : reset." << std::endl);
      busses_ = BusMap();
      branches_ = BranchVec();
   }

   void PowerFlowNR::validate()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : validate." << std::endl);

      // Make Nodes:
      NodeVec PQNodes = NodeVec();
      NodeVec PVNodes = NodeVec();
      NodeVec SLNodes = NodeVec();
      for (auto & busPair : busses_)
      {
         BusNR & bus = *busPair.second;
         NodeVec * vec = nullptr;
         if (bus.type_ == BusType::PQ)
         {
            vec = &PQNodes;
         }
         else if (bus.type_ == BusType::PV)
         {
            vec = &PVNodes;
         }
         else if (bus.type_ == BusType::SL)
         {
            vec = &SLNodes;
         }
         else
         {
            error() << "Unsupported bus type " << busType2Str(bus.type_) << std::endl;
            abort();
         }
         for (NodeNR * node : bus.nodes_)
         {
            vec->push_back(node);
         }
      }

      // Determine sizes:
      nPQ_ = PQNodes.size();
      nPV_ = PVNodes.size();
      nSL_ = SLNodes.size();
      assert(nSL_ > 0); // TODO: What is correct here?

      // Insert nodes into ordered list of all nodes. Be careful of ordering!
      nodes_ = NodeVec();
      nodes_.reserve(nNode());
      nodes_.insert(nodes_.end(), SLNodes.begin(), SLNodes.end());
      nodes_.insert(nodes_.end(), PQNodes.begin(), PQNodes.end());
      nodes_.insert(nodes_.end(), PVNodes.begin(), PVNodes.end());
      // Index all nodes:
      for (int i = 0; i < nNode(); ++i)
      {
         nodes_[i]->idx_ = i;
      }

      Y_.resize(nNode(), nNode(), false);

      // Branch admittances:
      for (BranchNR * branch : branches_)
      {
         auto it0 = busses_.find(branch->ids_[0]);
         if (it0 == busses_.end())
         {
            error() << "Branch " << branch->ids_[0] << " " << branch->ids_[1] << " contains a non-existent bus "
                    << branch->ids_[0] << std::endl;
            abort();
         }
         auto it1 = busses_.find(branch->ids_[1]);
         if (it1 == busses_.end())
         {
            error() << "Branch " << branch->ids_[0] << " " << branch->ids_[1] << " contains a non-existent bus "
                    << branch->ids_[1] << std::endl;
            abort();
         }
         const BusNR * busses[] = {it0->second, it1->second};
         int nTerm = 2 * branch->nPhase_;

         // There is one link per distinct pair of bus/phase pairs.
         for (int i = 0; i < nTerm; ++i)
         {
            int busIdxI = i / branch->nPhase_; // 0 or 1
            int branchPhaseIdxI = i % branch->nPhase_; // 0 to nPhase of branch.
            const BusNR * busI = busses[busIdxI];
            int busPhaseIdxI = busI->phases_.phaseIndex(branch->phases_[busIdxI][branchPhaseIdxI]);
            const NodeNR * nodeI = busI->nodes_[busPhaseIdxI];
            int idxNodeI = nodeI->idx_;

            // Only count each diagonal element in branch->Y_ once!
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i) + nodeI->ys_;

            for (int k = i + 1; k < nTerm; ++k)
            {
               int busIdxK = k / branch->nPhase_; // 0 or 1
               int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
               const BusNR * busK = busses[busIdxK];
               int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
               const NodeNR * nodeK = busK->nodes_[busPhaseIdxK];
               int idxNodeK = nodeK->idx_;

               Y_(idxNodeI, idxNodeK) += branch->Y_(i, k);
               Y_(idxNodeK, idxNodeI) += branch->Y_(k, i);
            }
         }
      } // Loop over branches.
      G_ = real(Y_);
      B_ = imag(Y_);
      SGT_DEBUG(debug() << "Y_.nnz() = " << Y_.nnz() << std::endl);

      // Load quantities.
      Ic_.resize(nNode(), false);
      for (int i = 0; i < nNode(); ++i)
      {
         Ic_(i) = nodes_[i]->Ic_;
      }

      SGT_DEBUG(debug() << "PowerFlowNR : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   /// Initialize voltages:
   void PowerFlowNR::initV(ublas::vector<double> & Vr, ublas::vector<double> & Vi) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNR & node = *nodes_[i];
         Vr(i) = node.V_.real();
         Vi(i) = node.V_.imag();
      }
   }

   void PowerFlowNR::initS(ublas::vector<double> & P, ublas::vector<double> & Q) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNR & node = *nodes_[i];
         P(i) = node.S_.real();
         Q(i) = node.S_.imag();
      }
   }

   static void initJCBlock(const ublas::matrix_range<const ublas::compressed_matrix<double>> & G,
                           const ublas::matrix_range<const ublas::compressed_matrix<double>> & B,
                           ublas::compressed_matrix<double> & Jrr,
                           ublas::compressed_matrix<double> & Jri,
                           ublas::compressed_matrix<double> & Jir,
                           ublas::compressed_matrix<double> & Jii)
   {
      Jrr = -G; 
      Jri =  B; 
      Jir = -B; 
      Jii = -G; 
   }

   /// Set the part of J that doesn't update at each iteration.
   /** At this stage, we are treating J as if all busses were PQ. */
   void PowerFlowNR::initJC(Jacobian & JC) const
   {
      initJCBlock(project(G_, selPQFromAll(), selPQFromAll()), 
                  project(B_, selPQFromAll(), selPQFromAll()),
                  JC.IrPQ_VrPQ(),
                  JC.IrPQ_ViPQ(),
                  JC.IiPQ_VrPQ(),
                  JC.IiPQ_ViPQ());
      initJCBlock(project(G_, selPQFromAll(), selPVFromAll()), 
                  project(B_, selPQFromAll(), selPVFromAll()),
                  JC.IrPQ_VrPV(),
                  JC.IrPQ_ViPV(),
                  JC.IiPQ_VrPV(),
                  JC.IiPQ_ViPV());
      initJCBlock(project(G_, selPVFromAll(), selPQFromAll()), 
                  project(B_, selPVFromAll(), selPQFromAll()),
                  JC.IrPV_VrPQ(),
                  JC.IrPV_ViPQ(),
                  JC.IiPV_VrPQ(),
                  JC.IiPV_ViPQ());
      initJCBlock(project(G_, selPVFromAll(), selPVFromAll()), 
                  project(B_, selPVFromAll(), selPVFromAll()),
                  JC.IrPV_VrPV(),
                  JC.IrPV_ViPV(),
                  JC.IiPV_VrPV(),
                  JC.IiPV_ViPV());
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNR::calcf(ublas::vector<double> & f,
                           const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                           const ublas::vector<double> & P, const ublas::vector<double> & Q,
                           const ublas::vector<double> & M2PV) const
   {
      // PQ busses:
      const ublas::compressed_matrix<double> GPQ = project(G_, selPQFromAll(), selAllFromAll());
      const ublas::compressed_matrix<double> BPQ = project(B_, selPQFromAll(), selAllFromAll());

      const auto VrPQ = project(Vr, selPQFromAll());
      const auto ViPQ = project(Vi, selPQFromAll());

      const auto PPQ = project(P, selPQFromAll());
      const auto QPQ = project(Q, selPQFromAll());
      
      const auto IcrPQ = project(real(Ic_), selPQFromAll());
      const auto IciPQ = project(imag(Ic_), selPQFromAll());

      ublas::vector<double> M2PQ = element_prod(VrPQ, VrPQ) + element_prod(ViPQ, ViPQ);

      project(f, selIrPQFromf()) = element_div(element_prod(VrPQ, PPQ) + element_prod(ViPQ, QPQ), M2PQ)
                                 + IcrPQ - myProd(GPQ, Vr) + myProd(BPQ, Vi);
      project(f, selIiPQFromf()) = element_div(element_prod(ViPQ, PPQ) - element_prod(VrPQ, QPQ), M2PQ)
                                 + IciPQ - myProd(GPQ, Vi) - myProd(BPQ, Vr);
      
      // PV busses. Note that these differ in that M2PV is considered a constant.
      const auto GPV = project(G_, selPVFromAll(), selAllFromAll());
      const auto BPV = project(B_, selPVFromAll(), selAllFromAll());

      const auto VrPV = project(Vr, selPVFromAll());
      const auto ViPV = project(Vi, selPVFromAll());

      const auto PPV = project(P, selPVFromAll());
      const auto QPV = project(Q, selPVFromAll());
      
      const auto IcrPV = project(real(Ic_), selPVFromAll());
      const auto IciPV = project(imag(Ic_), selPVFromAll());

      project(f, selIrPVFromf()) = element_div(element_prod(VrPV, PPV) + element_prod(ViPV, QPV), M2PV)
                                 + IcrPV - myProd(GPV, Vr) + myProd(BPV, Vi);
      project(f, selIiPVFromf()) = element_div(element_prod(ViPV, PPV) - element_prod(VrPV, QPV), M2PV)
                                 + IciPV - myProd(GPV, Vi) - myProd(BPV, Vr);
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNR::updateJ(Jacobian & J, const Jacobian & JC,
                             const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                             const ublas::vector<double> & P, const ublas::vector <double> & Q,
                             const ublas::vector<double> & M2PV) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in JC at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.

      // Reset PV Vi columns, since these get messed with:
      J.IrPQ_ViPV() = JC.IrPQ_ViPV();
      J.IiPQ_ViPV() = JC.IiPQ_ViPV();
      J.IrPV_ViPV() = JC.IrPV_ViPV();
      J.IiPV_ViPV() = JC.IiPV_ViPV();

      // Block diagonal:
      for (int i = 0; i < nPQ_; ++i)
      {
         int iPQi = iPQ(i);

         double PVr_p_QVi = P(iPQi) * Vr(iPQi) + Q(iPQi) * Vi(iPQi);
         double PVi_m_QVr = P(iPQi) * Vi(iPQi) - Q(iPQi) * Vr(iPQi);
         double M2 = Vr(iPQi) * Vr(iPQi) + Vi(iPQi) * Vi(iPQi);
         double M4 = M2 * M2;
         double VrdM4 = Vr(iPQi) / M4;
         double VidM4 = Vi(iPQi) / M4;
         double PdM2 = P(iPQi) / M2;
         double QdM2 = Q(iPQi) / M2;

         J.IrPQ_VrPQ()(i, i) = JC.IrPQ_VrPQ()(i, i) - (2 * VrdM4 * PVr_p_QVi) + PdM2;
         J.IrPQ_ViPQ()(i, i) = JC.IrPQ_ViPQ()(i, i) - (2 * VidM4 * PVr_p_QVi) + QdM2;
         J.IiPQ_VrPQ()(i, i) = JC.IiPQ_VrPQ()(i, i) - (2 * VrdM4 * PVi_m_QVr) - QdM2;
         J.IiPQ_ViPQ()(i, i) = JC.IiPQ_ViPQ()(i, i) - (2 * VidM4 * PVi_m_QVr) + PdM2;
      }

      // For PV busses, M^2 is constant, and therefore we can write the Jacobian more simply.
      for (int i = 0; i < nPV_; ++i)
      {
         int iPVi = iPV(i);

         J.IrPV_VrPV()(i, i) = JC.IrPV_VrPV()(i, i) + P(iPVi) / M2PV(i); // Could -> JC if we wanted.
         J.IrPV_ViPV()(i, i) = JC.IrPV_ViPV()(i, i) + Q(iPVi) / M2PV(i);
         J.IiPV_VrPV()(i, i) = JC.IiPV_VrPV()(i, i) - Q(iPVi) / M2PV(i);
         J.IiPV_ViPV()(i, i) = JC.IiPV_ViPV()(i, i) + P(iPVi) / M2PV(i);
      }

      // Set the PV Q columns in the Jacobian. They are diagonal.
      const auto VrPV = project(Vr, selPVFromAll());
      const auto ViPV = project(Vi, selPVFromAll());
      for (int i = 0; i < nPV_; ++i)
      {
         J.IrPV_QPV()(i, i) = ViPV(i) / M2PV(i);
         J.IiPV_QPV()(i, i) = -VrPV(i) / M2PV(i);
      }
   }

   // Modify J and f to take into account PV busses.
   void PowerFlowNR::modifyForPV(Jacobian & J, ublas::vector<double> & f,
                                 const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                                 const ublas::vector<double> & M2PV)
   {
      const auto VrPV = project(Vr, selPVFromAll());
      const auto ViPV = project(Vi, selPVFromAll());

      typedef ublas::vector_slice<ublas::vector<double>> VecSel;
      typedef ublas::matrix_column<ublas::compressed_matrix<double>> Column;
      auto mod = [](VecSel fProj, Column colViPV, const Column colVrPV, double fMult, double colViPVMult)
      {
         for (auto it = colVrPV.begin(); it != colVrPV.end(); ++it)
         {
            int idx = it.index();
            fProj(idx) += colVrPV(idx) * fMult;
            colViPV(idx) += colVrPV(idx) * colViPVMult;
         }
      };

      for (int k = 0; k < nPV_; ++k)
      { 
         double fMult = (0.5 * (M2PV(k) - VrPV(k) * VrPV(k) - ViPV(k) * ViPV(k)) / VrPV(k));
         double colViPVMult = -ViPV(k) / VrPV(k);

         mod(project(f, selIrPQFromf()), column(J.IrPQ_ViPV(), k), column(J.IrPQ_VrPV(), k), fMult, colViPVMult);
         mod(project(f, selIiPQFromf()), column(J.IiPQ_ViPV(), k), column(J.IiPQ_VrPV(), k), fMult, colViPVMult);
         mod(project(f, selIrPVFromf()), column(J.IrPV_ViPV(), k), column(J.IrPV_VrPV(), k), fMult, colViPVMult);
         mod(project(f, selIiPVFromf()), column(J.IiPV_ViPV(), k), column(J.IiPV_VrPV(), k), fMult, colViPVMult);
      }
   }

   void PowerFlowNR::calcJMatrix(ublas::compressed_matrix<double> & JMat, const Jacobian & J) const
   {
      Array<int, 4> ibInd = {0, 1, 2, 3};
      Array<int, 4> kbInd = {0, 1, 3, 4}; // Skip VrPV, since it doesn't appear as a variable.
      Array<ublas::slice, 4> sl1Vec = {selIrPQFromf(), selIiPQFromf(), selIrPVFromf(), selIiPVFromf()};
      Array<ublas::slice, 4> sl2Vec = {selVrPQFromx(), selViPQFromx(), selViPVFromx(), selQPVFromx()};

      JMat = ublas::compressed_matrix<double>(nVar(), nVar());

      // Loop over all blocks in J.
      for (int ib = 0; ib < 4; ++ib)
      {
         ublas::slice sl1 = sl1Vec[ib];
         for (int kb = 0; kb < 4; ++kb)
         {
            ublas::slice sl2 = sl2Vec[kb];
            const ublas::compressed_matrix<double> & block = J.blocks_[ibInd[ib]][kbInd[kb]];

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

   bool PowerFlowNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : solve." << std::endl);

      Stopwatch stopwatch;
      Stopwatch stopwatchTot;

      double durationInitSetup;
      double durationCalcf;
      double durationUpdateJ;
      double durationModifyForPV;
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

      ublas::vector<double> M2PV = element_prod(project(Vr, selPVFromAll()), project(Vr, selPVFromAll()))
                                 + element_prod(project(Vi, selPVFromAll()), project(Vi, selPVFromAll()));

      ublas::vector<double> P(nNode());
      ublas::vector<double> Q(nNode());
      initS(P, Q);

      Jacobian JC(nPQ_, nPV_); ///< The part of J that doesn't update at each iteration.
      initJC(JC);

      ublas::vector<double> f(nVar()); ///< Current mismatch function.

      Jacobian J = JC; ///< Jacobian, d f_i / d x_i.

      bool wasSuccessful = false;
      double err = 0;
      int niter;

      stopwatch.stop(); durationInitSetup = stopwatch.seconds();

      for (niter = 0; niter < maxiter; ++niter)
      {
         SGT_DEBUG(debug() << "\tIteration = " << niter << std::endl);

         stopwatch.reset(); stopwatch.start();
         calcf(f, Vr, Vi, P, Q, M2PV);

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
         updateJ(J, JC, Vr, Vi, P, Q, M2PV);
         stopwatch.stop(); durationUpdateJ += stopwatch.seconds();

         stopwatch.reset(); stopwatch.start();
         modifyForPV(J, f, Vr, Vi, M2PV);
         stopwatch.stop(); durationModifyForPV += stopwatch.seconds();

         // Construct the full Jacobian from J, which contains the block structure.
         stopwatch.reset(); stopwatch.start();
         ublas::compressed_matrix<double> JMat; calcJMatrix(JMat, J);
         stopwatch.stop(); durationConstructJMat += stopwatch.seconds();

         SGT_DEBUG
         (
            debug() << "\tBefore KLUSolve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
            debug() << "\tBefore KLUSolve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
            debug() << "\tBefore KLUSolve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl;
            debug() << "\tBefore KLUSolve: P   = " << std::setprecision(5) << std::setw(9) << P << std::endl;
            debug() << "\tBefore KLUSolve: Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl;
            debug() << "\tBefore KLUSolve: f   = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            debug() << "\tBefore KLUSolve: J   = " << std::endl;
            for (int i = 0; i < nVar(); ++i)
            {
               debug() << "\t\t" << std::setprecision(5) << std::setw(9) << row(JMat, i) << std::endl;
            }
         );

         stopwatch.reset(); stopwatch.start();
         ublas::vector<double> x;
         bool ok = KLUSolve(JMat, -f, x);
         stopwatch.stop(); durationSolve += stopwatch.seconds();

         SGT_DEBUG(debug() << "\tAfter KLUSolve: ok = " << ok << std::endl); 
         SGT_DEBUG(debug() << "\tAfter KLUSolve: x  = " << std::setprecision(5) << std::setw(9) << x << std::endl);
         if (!ok)
         {
            error() << "KLUSolve failed." << std::endl;
            abort();
         }

         stopwatch.reset(); stopwatch.start();
         // Update the current values of V from the solution:
         project(Vr, selPQFromAll()) += project(x, selVrPQFromx());
         project(Vi, selPQFromAll()) += project(x, selViPQFromx());

         // Explicitly deal with the voltage magnitude constraint by updating VrPV by hand.
         auto VrPV = project(Vr, selPVFromAll());
         auto ViPV = project(Vi, selPVFromAll());
         const auto DeltaViPV = project(x, selViPVFromx());
         VrPV += element_div(M2PV - element_prod(VrPV, VrPV) - element_prod(ViPV, ViPV) 
                             - 2 * element_prod(ViPV, DeltaViPV), 2 * VrPV);
         ViPV += DeltaViPV;

         // Update Q for PV busses based on the solution.
         project(Q, selPVFromAll()) += project(x, selQPVFromx());

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
         auto SSL = project(S, selSLFromAll());

         auto VSL = project(V, selSLFromAll());
         auto IcSL = project(Ic_, selSLFromAll());

         auto YStar = conj(project(Y_, selSLFromAll(), selAllFromAll()));
         auto VStar = conj(V);
         auto IcStar = conj(project(Ic_, selSLFromAll()));

         SSL = element_prod(VSL, prod(YStar, VStar)) - element_prod(VSL, IcStar);

         // Update nodes and busses.
         for (int i = 0; i < nNode(); ++i)
         {
            NodeNR * node = nodes_[i];
            node->V_ = V(i);
            node->S_ = S(i);
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
         }
      }
      else
      {
         warning() << "PowerFlowNR: Newton-Raphson method failed to converge." << std::endl; 
      }

      stopwatchTot.stop(); durationTot = stopwatchTot.seconds();

      message() << "PowerFlowNR: successful = " << wasSuccessful << ", error = " << err 
                << ", iterations = " << niter << ", total time = " << durationTot << "." << std::endl;
      SGT_DEBUG(debug() << "PowerFlowNR: -----------------------   " << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: init setup time         = " << durationInitSetup << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: time in calcf           = " << durationCalcf << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: time in updateJ         = " << durationUpdateJ << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: time in modifyForPV     = " << durationModifyForPV << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: time to construct JMat  = " << durationConstructJMat << " s." << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: solve time              = " << durationSolve << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: time to update iter     = " << durationUpdateIter << std::endl);
      SGT_DEBUG(debug() << "PowerFlowNR: -----------------------   " << std::endl);

      return wasSuccessful;
   }

   void PowerFlowNR::printProblem()
   {
      debug() << "PowerFlowNR::printProblem()" << std::endl;
      debug() << "\tNodes:" << std::endl;
      for (const NodeNR * nd : nodes_)
      {
         debug() << "\t\tNode:" << std::endl;
         debug() << "\t\t\tId    : " << nd->bus_->id_ << std::endl;
         debug() << "\t\t\tType  : " << nd->bus_->type_ << std::endl;
         debug() << "\t\t\tPhase : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
         debug() << "\t\t\tV     : " << nd->V_ << std::endl;
         debug() << "\t\t\tS     : " << nd->S_ << std::endl;
         debug() << "\t\t\tys    : " << nd->ys_ << std::endl;
         debug() << "\t\t\tIc    : " << nd->Ic_ << std::endl;
      }
      debug() << "\tBranches:" << std::endl;
      for (const BranchNR * branch : branches_)
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
