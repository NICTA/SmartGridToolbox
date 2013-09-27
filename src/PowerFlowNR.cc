#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   UblasRange selPQ();
   UblasRange selPV();
   UblasRange selSL();
   UblasRange selPQPV();
   UblasRange selAll();

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
      VPV_.resize(nPV_, false);
      for (int i = 0; i < nPV_; ++i)
      {
         PPV_(i) = PVNodes[i]->S_.real();
         VPV_(i) = abs(PVNodes[i]->V_);
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

      P0_.resize(nNode(), false);
      Q0_.resize(nNode(), false);
      P1_.resize(nNode(), false);
      Q1_.resize(nNode(), false);
      P2_.resize(nNode(), false);
      Q2_.resize(nNode(), false);

      // Set up the load model:
      for (int i = 0; i < nNode(); ++i)
      {
         P0_[i] = nodes_[i]->S_.real();
         Q0_[i] = nodes_[i]->S_.imag();
         P1_[i] = nodes_[i]->I_.real();
         Q1_[i] = nodes_[i]->I_.imag();
         P2_[i] = nodes_[i]->Y_.real();
         Q2_[i] = nodes_[i]->Y_.imag();
      }

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
            Y(idxNodeI, idxNodeI) += branch->Y_(i, i);

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

      SGT_DEBUG(debug() << "PowerFlowNR : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   /// Initialize x:
   void PowerFlowNR::initV(UblasVector<double> & Vr, UblasVector<double> & Vi) const
   {
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNR & node = *nodes_[i];
         Vr(i) = node.V_.real();
         Vi(i) = node.V_.imag();
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   void PowerFlowNR::initJConst(UblasCMatrix<double> & JConst) const
   {
      project(JConst, selPQi(), selPQi()) = project(B_, selPQ(), selPQ());
      project(JConst, selPQi(), selPQr()) = project(G_, selPQ(), selPQ());
      project(JConst, selPQr(), selPQi()) = project(G_, selPQ(), selPQ());
      project(JConst, selPQr(), selPQr()) = -project(B_, selPQ(), selPQ());
      // TODO: PV parts.
   }

   void PowerFlowNR::updatef(UblasVector<double> & f,
                             const UblasVector<double> & Vr, const UblasVector<double> & Vi) const
   {
      const auto GRng = project(G_, selPQPV(), selAll());
      const auto BRng = project(B_, selPQPV(), selAll());

      const auto VrPQPV = project(Vr, selPQPV());
      const auto ViPQPV = project(Vi, selPQPV());
      const auto VrPQ = project(Vr, selPQ());
      const auto ViPQ = project(Vi, selPQ());
      const auto VrPV = project(Vr, selPV());
      const auto ViPV = project(Vi, selPV());

      UblasVector<double> M2PQPV = element_prod(VrPQPV, VrPQPV) + element_prod(ViPQPV, ViPQPV);
      const auto M2PQ = project(M2PQPV, selPQ());
      const auto M2PV = project(M2PQPV, selPV());

      UblasVector<double> Icalci = prod(GRng, Vi) + prod(BRng, Vr);
      UblasVector<double> Icalcr = prod(GRng, Vr) - prod(BRng, Vi);

      UblasVector<double> Pcalc = element_prod(VrPQPV, Icalcr) + element_prod(ViPQPV, Icalci);
      UblasVector<double> Qcalc = element_prod(ViPQPV, Icalcr) - element_prod(VrPQPV, Icalci);

      UblasVector<double> DeltaPPQ = PPQ_ - project(Pcalc, selPQ());
      UblasVector<double> DeltaQPQ = QPQ_ - project(Qcalc, selPQ());

      UblasVector<double> DeltaPPV = PPV_ - project(Pcalc, selPV());

      project(f, selPQr()) = element_div(element_prod(VrPQ, DeltaPPQ) + element_prod(ViPQ, DeltaQPQ), M2PQ);
      project(f, selPQi()) = element_div(element_prod(ViPQ, DeltaPPQ) - element_prod(VrPQ, DeltaQPQ), M2PQ);

      project(f, selPVr()) = element_div(element_prod(VrPV, DeltaPPV), M2PV);
      project(f, selPVi()) = element_div(element_prod(ViPV, DeltaPPV), M2PV);
   }

   void PowerFlowNR::updateJ(UblasCMatrix<double> & J, const UblasCMatrix<double> & JConst,
                             const UblasVector<double> Vr, const UblasVector<double> Vi) const
   {
      for (int i = 0; i < nPQPV(); ++i)
      {
         double Vr2 = Vr(i) * Vr(i);
         double Vi2 = Vi(i) * Vi(i);
         double VrVi = Vr(i) * Vi(i);
         double M2 = Vr2 + Vi2;
         double M4 = M2 * M2;
         double M3 = sqrt(M4 * M2);

         J(2*i  , 2*i  ) = JConst(2*i  , 2*i  )
                         - (Q0_(i) * (Vr2 - Vi2) - 2 * P0_(i) * VrVi) / M4
                         - (P1_(i) * VrVi + Q1_(i) * Vi2) / M3
                         - Q2_(i);

         J(2*i  , 2*i+1) = JConst(2*i  , 2*i+1)
                         - (P0_(i) * (Vr2 - Vi2) + 2 * Q0_(i) * VrVi) / M4
                         + (Q1_(i) * VrVi + P1_(i) * Vr2) / M3
                         + P2_(i);

         J(2*i+1, 2*i  ) = JConst(2*i+1, 2*i  )
                         - (P0_(i) * (Vi2 - Vr2) - 2 * Q0_(i) * VrVi) / M4
                         - (Q1_(i) * VrVi - P1_(i) * Vi2) / M3
                         + P2_(i);

         J(2*i+1, 2*i+1) = JConst(2*i+1, 2*i+1)
                         - (Q0_(i) * (Vr2 - Vi2) - 2 * P0_(i) * VrVi) / M4
                         - (P1_(i) * VrVi - Q1_(i) * Vr2) / M3
                         + Q2_(i);

         // TODO: PV nodes.
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

      UblasCMatrix<double> JConst(nVar(), nVar()); ///< The part of J that doesn't update at each iteration.
      initJConst(JConst);

      UblasVector<double> f(nVar()); ///< Current mismatch function.

      UblasCMatrix<double> J = JConst; ///< Jacobian, d f_i / d x_i.
      J = JConst; // We only need to redo the elements that we mess with!

      bool wasSuccessful = false;
      for (int i = 0; i < maxiter; ++ i)
      {
         SGT_DEBUG(debug() << "\tIteration = " << i << std::endl);

         updatef(f, Vr, Vi);
         UblasVector<double> f2 = element_prod(f, f);
         double err = *std::max_element(f2.begin(), f2.end());
         SGT_DEBUG(debug() << "\tError = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(debug() << "\tSuccess at iteration " << i << ". Error = " << err << std::endl);
            wasSuccessful = true;
            break;
         }

         updateJ(J, JConst, Vr, Vi);

         UblasVector<double> rhs;

         SGT_DEBUG
         (
            debug() << "\tBefore KLUSolve: f = " << std::setw(8) << f << std::endl;
            debug() << "\tBefore KLUSolve: J = " << std::endl;
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
         project(Vr, selPQ()) += project(rhs, selPQr());
         project(Vi, selPQ()) += project(rhs, selPQi());
         project(Vr, selPV()) -= element_prod(element_div(project(Vi, selPV()), project(Vr, selPV())), 
                                              project(rhs, selPVi())); 
         project(Vi, selPV()) += project(rhs, selPVi());
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
