#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   BusNR::BusNR(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
                const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S) :
      id_(id),
      type_(type),
      phases_(phases),
      V_(V),
      Y_(Y),
      I_(I),
      S_(S)
   {
      assert(V.size() == phases.size());
      assert(Y.size() == phases.size());
      assert(I.size() == phases.size());
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
      Y_(bus.Y_(phaseIdx)),
      I_(bus.I_(phaseIdx)),
      S_(bus.S_(phaseIdx)),
      idx_(-1)
   {
      // Empty.
   }

   BranchNR::BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1, 
                      const UblasMatrix<Complex> & Y) :
      nPhase_(phases0.size()),
      ids_{id0, id1},
      phases_{phases0, phases1},
      Y_(Y)
   {
      assert(phases1.size() == nPhase_);
      int nTerm = 2 * nPhase_;
      assert(Y.size1() == nTerm);
      assert(Y.size2() == nTerm);
   }

   PowerFlowNR::~PowerFlowNR()
   {
      for (auto pair : busses_) delete pair.second;
      for (auto branch : branches_) delete branch;
   }

   void PowerFlowNR::addBus(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
         const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S)
   {
      SGT_DEBUG(debug() << "PowerFlowNR : add bus " << id << std::endl);
      busses_[id] = new BusNR(id, type, phases, V, Y, I, S);
   }

   void PowerFlowNR::addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                               const UblasMatrix<Complex> & Y)
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

      // Insert nodes into ordered list of all nodes:
      nodes_ = NodeVec();
      nodes_.reserve(nNode());
      nodes_.insert(nodes_.end(), PQNodes.begin(), PQNodes.end());
      nodes_.insert(nodes_.end(), PVNodes.begin(), PVNodes.end());
      nodes_.insert(nodes_.end(), SLNodes.begin(), SLNodes.end());
      // Index all nodes:
      for (int i = 0; i < nNode(); ++i)
      {
         nodes_[i]->idx_ = i;
      }

      // Size and set up arrays of constant input quantities:
      PPQ_.resize(nPQ_, false);
      QPQ_.resize(nPQ_, false);
      for (int i = 0; i < nPQ_; ++i)
      {
         PPQ_(i) = PQNodes[i]->S_.real();
         QPQ_(i) = PQNodes[i]->S_.imag();
      }

      PPV_.resize(nPV_, false);
      MPV_.resize(nPV_, false);
      for (int i = 0; i < nPV_; ++i)
      {
         PPV_(i) = PVNodes[i]->S_.real();
         MPV_(i) = abs(PVNodes[i]->V_);
      }

      VSLr_.resize(nSL_, false);
      VSLi_.resize(nSL_, false);
      for (int i = 0; i < nSL_; ++i)
      {
         VSLr_(i) = SLNodes[i]->V_.real();
         VSLi_(i) = SLNodes[i]->V_.imag();
      }

      G_.resize(nNode(), nNode(), false);
      B_.resize(nNode(), nNode(), false);

      // Build the bus admittance matrix:
      UblasCMatrix<Complex> Y(nNode(), nNode()); // Complex Y matrix.
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
            Y(idxNodeI, idxNodeI) += branch->Y_(i, i) + nodeI->Y_;

            for (int k = i + 1; k < nTerm; ++k)
            {
               int busIdxK = k / branch->nPhase_; // 0 or 1
               int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
               const BusNR * busK = busses[busIdxK];
               int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
               const NodeNR * nodeK = busK->nodes_[busPhaseIdxK];
               int idxNodeK = nodeK->idx_;

               Y(idxNodeI, idxNodeK) += branch->Y_(i, k);
               Y(idxNodeK, idxNodeI) += branch->Y_(k, i);
            }
         }
      } // Loop over branches.

      // And set G_ and B_:
      G_ = real(Y);
      B_ = imag(Y);

      // Load quantities.
      IcR_.resize(nNode(), false);
      IcI_.resize(nNode(), false);
      Pc_.resize(nNode(), false);
      Qc_.resize(nNode(), false);
      for (int i = 0; i < nNode(); ++i)
      {
         IcR_(i) = nodes_[i]->I_.real();
         IcI_(i) = nodes_[i]->I_.imag();
         Pc_(i) = nodes_[i]->S_.real();
         Qc_(i) = nodes_[i]->S_.imag();
      }

      SGT_DEBUG(debug() << "PowerFlowNR : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   /// Initialize voltages:
   void PowerFlowNR::initV(UblasVector<double> & Vr, UblasVector<double> & Vi) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNR & node = *nodes_[i];
         Vr(i) = node.V_.real();
         Vi(i) = node.V_.imag();
      }
   }

   void PowerFlowNR::initS(UblasVector<double> & P, UblasVector<double> & Q) const
   {
      project(P, selPQ()) = PPQ_;
      project(Q, selPQ()) = QPQ_;
      project(P, selPV()) = PPV_;
      auto QPV = project(Q, selPV());
      auto PSL = project(P, selSL());
      auto QSL = project(Q, selSL());
      for (int i = 0; i < nPV_; ++i)
      {
         QPV(i) = 0;
      }
      for (int i = 0; i < nSL_; ++i)
      {
         PSL(i) = 0;
         QSL(i) = 0;
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   void PowerFlowNR::initJC(UblasCMatrix<double> & JC) const
   {
      project(JC, selfIrPQ(), selVrPQ()) = -project(G_, selPQ(), selPQ());
      project(JC, selfIrPQ(), selViPQ()) =  project(B_, selPQ(), selPQ());
      project(JC, selfIiPQ(), selVrPQ()) = -project(B_, selPQ(), selPQ());
      project(JC, selfIiPQ(), selViPQ()) = -project(G_, selPQ(), selPQ());

      project(JC, selfIrPV(), selVrPQ()) = -project(G_, selPV(), selPQ());
      project(JC, selfIrPV(), selViPQ()) =  project(B_, selPV(), selPQ());
      project(JC, selfIiPV(), selVrPQ()) = -project(B_, selPV(), selPQ());
      project(JC, selfIiPV(), selViPQ()) = -project(G_, selPV(), selPQ());

      project(JC, selfIrPQ(), selViPV()) =  project(B_, selPQ(), selPV());
      project(JC, selfIiPQ(), selViPV()) = -project(G_, selPQ(), selPV());

      project(JC, selfIrPV(), selViPV()) =  project(B_, selPV(), selPV());
      project(JC, selfIiPV(), selViPV()) = -project(G_, selPV(), selPV());
   }

   void PowerFlowNR::updatef(UblasVector<double> & f,
                             const UblasVector<double> & Vr, const UblasVector<double> & Vi,
                             const UblasVector<double> & P, const UblasVector<double> & Q) const
   {
      const auto GRng = project(G_, selPQPV(), selAll());
      const auto BRng = project(B_, selPQPV(), selAll());

      const auto VrPQPV = project(Vr, selPQPV());
      const auto ViPQPV = project(Vi, selPQPV());
      
      const auto PPQPV = project(P, selPQPV());
      const auto QPQPV = project(Q, selPQPV());

      UblasVector<double> M2PQPV = element_prod(VrPQPV, VrPQPV) + element_prod(ViPQPV, ViPQPV);

      auto Ir = element_div(element_prod(VrPQPV, PPQPV) + element_prod(ViPQPV, QPQPV), M2PQPV)
                            + project(IcR_, selPQPV())
                            - prod(GRng, Vr) + prod(BRng, Vi);
      auto Ii = element_div(element_prod(ViPQPV, PPQPV) - element_prod(VrPQPV, QPQPV), M2PQPV)
                            + project(IcI_, selPQPV())
                            - prod(GRng, Vi) - prod(BRng, Vr);
      project(f, selfIrPQ()) = project(Ir, selPQ());
      project(f, selfIiPQ()) = project(Ii, selPQ());
      project(f, selfIrPV()) = project(Ir, selPV());
      project(f, selfIiPV()) = project(Ii, selPV());
   }

   void PowerFlowNR::updateJ(UblasCMatrix<double> & J, const UblasCMatrix<double> & JC,
                             const UblasVector<double> Vr, const UblasVector<double> Vi,
                             const UblasVector<double> P, const UblasVector<double> Q) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in JC at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.
      // Thus we need to do (PQ,PQ) diagonal and (*, PV) columns.
    
      auto VrPQ = project(Vr, selPQ());
      auto ViPQ = project(Vi, selPQ());
      auto PPQ = project(P, selPQ());
      auto QPQ = project(Q, selPQ());

      auto VrPV = project(Vr, selPV());
      auto ViPV = project(Vi, selPV());
      auto PPV = project(P, selPV());
      auto QPV = project(Q, selPV());

      // (PQ, PQ):
      auto JPQIrPQVr = project(J, selfIrPQ(), selVrPQ());
      auto JPQIrPQVi = project(J, selfIrPQ(), selViPQ());
      auto JPQIiPQVr = project(J, selfIiPQ(), selVrPQ());
      auto JPQIiPQVi = project(J, selfIiPQ(), selViPQ());

      auto JCPQIrPQVr = project(JC, selfIrPQ(), selVrPQ());
      auto JCPQIrPQVi = project(JC, selfIrPQ(), selViPQ());
      auto JCPQIiPQVr = project(JC, selfIiPQ(), selVrPQ());
      auto JCPQIiPQVi = project(JC, selfIiPQ(), selViPQ());
      
      // (PQ, PV)
      auto JPQIrPVQ = project(J, selfIrPQ(), selQPV());
      auto JPQIrPVVi = project(J, selfIrPQ(), selViPV());
      auto JPQIiPVQ = project(J, selfIiPQ(), selQPV());
      auto JPQIiPVVi = project(J, selfIiPQ(), selViPV());

      auto JCPQIrPVQ = project(JC, selfIrPQ(), selQPV());
      auto JCPQIrPVVi = project(JC, selfIrPQ(), selViPV());
      auto JCPQIiPVQ = project(JC, selfIiPQ(), selQPV());
      auto JCPQIiPVVi = project(JC, selfIiPQ(), selViPV());

      auto GPQPV = project(G_, selPQ(), selPV());
      auto BPQPV = project(G_, selPQ(), selPV());

      // (PV, PQ):
      auto JPVIrPQIr = project(J, selfIrPV(), selVrPQ());
      auto JPVIrPQIi = project(J, selfIrPV(), selViPQ());
      auto JPVIiPQIr = project(J, selfIiPV(), selVrPQ());
      auto JPVIiPQIi = project(J, selfIiPV(), selViPQ());

      auto JCPVIrPQIr = project(JC, selfIrPV(), selVrPQ());
      auto JCPVIrPQIi = project(JC, selfIrPV(), selViPQ());
      auto JCPVIiPQIr = project(JC, selfIiPV(), selVrPQ());
      auto JCPVIiPQIi = project(JC, selfIiPV(), selViPQ());

      // (PV, PV):
      auto JPVIrPVQ = project(J, selfIrPV(), selQPV());
      auto JPVIrPVVi = project(J, selfIrPV(), selViPV());
      auto JPVIiPVQ = project(J, selfIiPV(), selQPV());
      auto JPVIiPVVi = project(J, selfIiPV(), selViPV());

      auto JCPVIrPVQ = project(JC, selfIrPV(), selQPV());
      auto JCPVIrPVVi = project(JC, selfIrPV(), selViPV());
      auto JCPVIiPVQ = project(JC, selfIiPV(), selQPV());
      auto JCPVIiPVVi = project(JC, selfIiPV(), selViPV());

      auto GPVPV = project(G_, selPV(), selPV());
      auto BPVPV = project(G_, selPV(), selPV());

      // (PQ, PQ) block diagonal:
      for (int i = 0; i < nPQ_; ++i)
      {
         double PVr_p_QVi = PPQ(i) * VrPQ(i) + QPQ(i) * ViPQ(i);
         double PVi_m_QVr = PPQ(i) * ViPQ(i) - QPQ(i) * VrPQ(i);
         double M2 = VrPQ(i) * VrPQ(i);
         double M4 = M2 * M2;
         double VrdM4 = VrPQ(i) / M4;
         double VidM4 = ViPQ(i) / M4;
         double PdM2 = PPQ(i) / M2;
         double QdM2 = QPQ(i) / M2;

         JPQIrPQVr(i, i) = JCPQIrPQVr(i, i) - (2 * VrdM4 * PVr_p_QVi) + PdM2;
         JPQIrPQVi(i, i) = JCPQIrPQVi(i, i) - (2 * VidM4 * PVr_p_QVi) + QdM2;
         JPQIiPQVr(i, i) = JCPQIiPQVr(i, i) - (2 * VrdM4 * PVi_m_QVr) - QdM2;
         JPQIiPQVi(i, i) = JCPQIiPQVi(i, i) - (2 * VidM4 * PVi_m_QVr) + PdM2;
      }

      // (PV, PV) block diagonal:
      for (int i = 0; i < nPV_; ++i)
      {
         double M2 = VrPV(i) * VrPV(i);

         JPVIrPVQ(i, i) = JCPVIrPVQ(i, i) + ViPV(i) / M2;
         JPVIiPVQ(i, i) = JCPVIiPVQ(i, i) - VrPV(i) / M2;
         JPVIrPVVi(i, i) = JCPVIrPVVi(i, i) + (QPV(i) - ViPV(i) * PPV(i) / VrPV(i)) / M2;
         JPVIiPVVi(i, i) = JCPVIiPVVi(i, i) + (PPV(i) + ViPV(i) * QPV(i) / VrPV(i)) / M2;
      }

      // (*, PV) columns:
      // TODO: vectorize.
      for (int k = 0; k < nPV_; ++k)
      {
         double mult = ViPV(k) / VrPV(k);
         // PQ PV:
         for (int i = 0; i < nPQ_; ++i)
         {
            JPQIrPVQ(i, k) = JCPQIrPVQ(i, k) + mult * GPQPV(i, k);
            JPQIiPVQ(i, k) = JCPQIiPVQ(i, k) + mult * BPQPV(i, k);
         }
         // PV PV:
         for (int i = 0; i < nPV_; ++i)
         {
            JPVIrPVQ(i, k) = JCPVIrPVQ(i, k) + mult * GPVPV(i, k);
            JPVIiPVQ(i, k) = JCPVIiPVQ(i, k) + mult * BPVPV(i, k);
         }
      }
   }

   bool PowerFlowNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : solve." << std::endl);

      const double tol = 1e-20;
      const int maxiter = 20;

      UblasVector<double> Vr(nNode());
      UblasVector<double> Vi(nNode());
      initV(Vr, Vi);

      UblasVector<double> P(nNode());
      UblasVector<double> Q(nNode());
      initS(P, Q);

      UblasCMatrix<double> JC(nVar(), nVar()); ///< The part of J that doesn't update at each iteration.
      initJC(JC);

      UblasVector<double> f(nVar()); ///< Current mismatch function.

      UblasCMatrix<double> J = JC; ///< Jacobian, d f_i / d x_i.
      J = JC; // We only need to redo the elements that we mess with!

      bool wasSuccessful = false;
      for (int i = 0; i < maxiter; ++ i)
      {
         SGT_DEBUG(debug() << "\tIteration = " << i << std::endl);

         updatef(f, Vr, Vi, P, Q);
         UblasVector<double> f2 = element_prod(f, f);
         double err = *std::max_element(f2.begin(), f2.end());
         SGT_DEBUG(debug() << "\tf  = " << std::setw(8) << f << std::endl);
         SGT_DEBUG(debug() << "\tError = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(debug() << "\tSuccess at iteration " << i << ". Error = " << err << std::endl);
            wasSuccessful = true;
            break;
         }

         updateJ(J, JC, Vr, Vi, P, Q);

         UblasVector<double> rhs;

         SGT_DEBUG
         (
            debug() << "\tBefore KLUSolve: Vr = " << std::setw(8) << Vr << std::endl;
            debug() << "\tBefore KLUSolve: Vi = " << std::setw(8) << Vi << std::endl;
            debug() << "\tBefore KLUSolve: f  = " << std::setw(8) << f << std::endl;
            debug() << "\tBefore KLUSolve: J  = " << std::endl;
            for (int i = 0; i < nVar(); ++i)
            {
               debug() << "\t\t" << std::setw(8) << row(J, i) << std::endl;
            }
         );

         bool ok = KLUSolve(J, f, rhs);
         SGT_DEBUG(debug() << "\tAfter KLUSolve: ok = " << ok << ", rhs = " << std::setw(8) << rhs << std::endl);
         if (!ok)
         {
            error() << "KLUSolve failed." << std::endl;
            abort();
         }

         // Update the current values of V from rhs:
         project(Vr, selPQ()) -= project(rhs, selVrPQ());
         project(Vi, selPQ()) -= project(rhs, selViPQ());
         project(Vr, selPV()) += element_prod(element_div(project(Vi, selPV()), project(Vr, selPV())), 
                                              project(rhs, selViPV())); 
         project(Vi, selPV()) -= project(rhs, selViPV());
         SGT_DEBUG(debug() << "\tUpdated Vr = " << std::setw(8) << Vr << std::endl);
         SGT_DEBUG(debug() << "\tUpdated Vi = " << std::setw(8) << Vi << std::endl);
      }
      if (wasSuccessful)
      {
         for (int i = 0; i < nNode(); ++i)
         {
            NodeNR * node = nodes_[i];
            node->V_ = {Vr(i), Vi(i)};
            node->bus_->V_[node->phaseIdx_] = node->V_;
         }
         // TODO: set power e.g. on slack bus. Set current injections. Set impedances to ground. 
      }
      SGT_DEBUG(debug() << "PowerFlowNR : solve finished. Was successful = " << wasSuccessful << std::endl);
      return wasSuccessful;
   }

   bool PowerFlowNR::printProblem()
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
         debug() << "\t\t\tY     : " << nd->Y_ << std::endl;
         debug() << "\t\t\tI     : " << nd->I_ << std::endl;
         debug() << "\t\t\tS     : " << nd->S_ << std::endl;
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
            debug() << "\t\t\t\t" << std::setw(16) << row(branch->Y_, i) << std::endl;
         }
      }
      debug() << "\tY:" << std::endl;
      for (int i = 0; i < G_.size1(); ++i)
      {
         debug() << "\t\t[" << std::setw(16) << std::left << Complex{G_(i, 0), B_(i, 0)};
         for (int k = 1; k < G_.size2(); ++k) 
         {
            debugStream() << " " << std::setw(16) << std::left << Complex{G_(i, k), B_(i, k)};
         }
         debugStream() << "]" << std::endl;
      }
   }
}
