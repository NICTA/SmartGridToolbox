#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   template<typename T> using UblasCMatrixRange = boost::numeric::ublas::matrix_range<UblasCMatrix<T>>;
   template<typename T> using UblasVectorRange = boost::numeric::ublas::vector_range<UblasVector<T>>;

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
      SLNodes_ = NodeVec();
      PQNodes_ = NodeVec();
      for (auto & busPair : busses_)
      {
         BusNR & bus = *busPair.second;
         NodeVec * vec = nullptr;
         if (bus.type_ == BusType::SL)
         {
            vec = &SLNodes_;
         }
         else if (bus.type_ == BusType::PQ)
         {
            vec = &PQNodes_;
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
      nSL_ = SLNodes_.size();
      nPQ_ = PQNodes_.size();
      assert(nSL_ > 0); // TODO: What is correct here?
      assert(nPQ_ > 0); // TODO: What is correct here?
      nNode_ = nSL_ + nPQ_;
      nVar_ = 2 * nPQ_;

      nodes_ = NodeVec();
      nodes_.reserve(nNode_);
      nodes_.insert(nodes_.end(), PQNodes_.begin(), PQNodes_.end());
      nodes_.insert(nodes_.end(), SLNodes_.begin(), SLNodes_.end());

      // Index all nodes (PQ ones come first):
      for (int i = 0; i < nNode_; ++i)
      {
         nodes_[i]->idx_ = i;
      }

      // Set array ranges:
      // Note Range goes from begin (included) to end (excluded).
      rPQ_ = UblasRange(0, nPQ_);
      rSL_ = UblasRange(nPQ_, nPQ_ + nSL_);
      rAll_ = UblasRange(0, nNode_);
      rx0_ = UblasRange(0, nPQ_);
      rx1_ = UblasRange(nPQ_, 2 * nPQ_);

      // Size all arrays:
      V0r_.resize(nSL_, false);
      V0i_.resize(nSL_, false);
      PPQ_.resize(nPQ_, false);
      QPQ_.resize(nPQ_, false);
      IrPQ_.resize(nPQ_, false);
      IiPQ_.resize(nPQ_, false);
      Vr_.resize(nNode_, false);
      Vi_.resize(nNode_, false);
      Y_.resize(nNode_, nNode_, false);
      G_.resize(nNode_, nNode_, false);
      B_.resize(nNode_, nNode_, false);
      x_.resize(nVar_, false);
      f_.resize(nVar_, false);
      J_.resize(nVar_, nVar_, false);
      JConst_.resize(nVar_, nVar_, false);

      // Build the bus admittance matrix:
      Y_.clear();
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
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i);

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

      // Add bus shunt admittances.
      for (auto pair : busses_)
      {
         for (const NodeNR * const node : pair.second->nodes_)
         {
            int nodeIdx = node->idx_;
            Y_(nodeIdx, nodeIdx) += node->Y_;
         }
      }

      for (int i = 0; i < nPQ_; ++i)
      {
         // Set the PPQ_ and QPQ_ arrays of real and reactive power on each terminal:
         PPQ_(i) = PQNodes_[i]->S_.real();
         QPQ_(i) = PQNodes_[i]->S_.imag();

         // Set the constant current arrays of real and reactive power on each terminal:
         IrPQ_(i) = PQNodes_[i]->I_.real();
         IiPQ_(i) = PQNodes_[i]->I_.imag();
      }

      for (int i = 0; i < nSL_; ++i)
      {
         V0r_(i) = SLNodes_[i]->V_.real();
         V0i_(i) = SLNodes_[i]->V_.imag();
      }

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);

      // Set the part of J that doesn't update at each iteration.
      JConst_.clear();
      UblasCMatrixRange<double>(JConst_, rx0_, rx0_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx0_, rx1_) = -UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx0_) = UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx1_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      J_ = JConst_; // We only need to redo the elements that we mess with!
      SGT_DEBUG(debug() << "PowerFlowNR : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   void PowerFlowNR::initx()
   {
      for (int i = 0; i < nPQ_; ++i)
      {
         const NodeNR & node = *PQNodes_[i];
         x_(i) = node.V_.real();
         x_(i + nPQ_) = node.V_.imag();
      }
   }

   void PowerFlowNR::updateNodeV()
   {
      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      UblasVectorRange<double>(Vr_, rPQ_) = UblasVectorRange<double>(x_, rx0_);
      UblasVectorRange<double>(Vi_, rPQ_) = UblasVectorRange<double>(x_, rx1_);
      UblasVectorRange<double>(Vr_, rSL_) = V0r_;
      UblasVectorRange<double>(Vi_, rSL_) = V0i_;
   }

   void PowerFlowNR::updateF()
   {
      UblasVectorRange<double> x0{x_, rx0_};
      UblasVectorRange<double> x1{x_, rx1_};

      UblasVector<double> M2 = element_prod(x0, x0) + element_prod(x1, x1);

      UblasCMatrixRange<double> GRng{G_, rPQ_, rAll_};
      UblasCMatrixRange<double> BRng{B_, rPQ_, rAll_};

      UblasVector<double> dr = element_div((-element_prod(PPQ_, x0) - element_prod(QPQ_, x1)), M2)
                               + prod(GRng, Vr_) - prod(BRng, Vi_) - IrPQ_;
      UblasVector<double> di = element_div((-element_prod(PPQ_, x1) + element_prod(QPQ_, x0)), M2)
                               + prod(GRng, Vi_) + prod(BRng, Vr_) - IiPQ_;

      UblasVectorRange<double>(f_, rx0_) = dr;
      UblasVectorRange<double>(f_, rx1_) = di;
   }

   void PowerFlowNR::updateJ()
   {
      UblasVectorRange<double> x0{x_, rx0_};
      UblasVectorRange<double> x1{x_, rx1_};

      UblasVector<double> M2PQ = element_prod(x0, x0) + element_prod(x1, x1);
      UblasVector<double> M4PQ = element_prod(M2PQ, M2PQ);

      for (int i = 0; i < nPQ_; ++i)
      {
         J_(i, i) = JConst_(i, i) +
            (-PPQ_(i) / M2PQ(i) + 2 * Vr_(i) * (PPQ_(i) * Vr_(i) + QPQ_(i) * Vi_(i)) / M4PQ(i));
         J_(i, i + nPQ_) = JConst_(i, i + nPQ_) +
            (-QPQ_(i) / M2PQ(i) + 2 * Vi_(i) * (PPQ_(i) * Vr_(i) + QPQ_(i) * Vi_(i)) / M4PQ(i));
         J_(i + nPQ_, i) = JConst_(i + nPQ_, i) +
            ( QPQ_(i) / M2PQ(i) + 2 * Vr_(i) * (PPQ_(i) * Vi_(i) - QPQ_(i) * Vr_(i)) / M4PQ(i));
         J_(i + nPQ_, i + nPQ_) = JConst_(i + nPQ_, i + nPQ_) +
            (-PPQ_(i) / M2PQ(i) + 2 * Vi_(i) * (PPQ_(i) * Vi_(i) - QPQ_(i) * Vr_(i)) / M4PQ(i));
      }
   }

   bool PowerFlowNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : solve." << std::endl);
      const double tol = 1e-20;
      const int maxiter = 20;
      initx();
      bool wasSuccessful = false;
      for (int i = 0; i < maxiter; ++ i)
      {
         SGT_DEBUG(debug() << "\tIteration = " << i << std::endl);
         updateNodeV();
         updateF();
         UblasVector<double> f2 = element_prod(f_, f_);
         double err = *std::max_element(f2.begin(), f2.end());
         SGT_DEBUG(debug() << "\tError = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(debug() << "\tSuccess at iteration " << i << ". Error = " << err << std::endl);
            wasSuccessful = true;
            break;
         }
         updateJ();
         UblasVector<double> rhs;

         SGT_DEBUG
         (
            debug() << "\tBefore KLUSolve: f = " << std::setw(8) << f_ << std::endl;
            debug() << "\tBefore KLUSolve: J = " << std::endl;
            for (int i = 0; i < nVar_; ++i)
            {
               debug() << "\t\t" << std::setw(8) << row(J_, i) << std::endl;
            }
         );

         bool ok = KLUSolve(J_, f_, rhs);
         SGT_DEBUG(debug() << "\tAfter KLUSolve: ok = " << ok << ", rhs = " << std::setw(8) << rhs << std::endl);
         if (!ok)
         {
            error() << "KLUSolve failed." << std::endl;
            abort();
         }
         x_ = x_ - rhs;
         SGT_DEBUG(debug() << "\tNew x = " << std::setw(8) << x_ << std::endl);
      }
      if (wasSuccessful)
      {
         for (int i = 0; i < nPQ_; ++i)
         {
            NodeNR * node = PQNodes_[i];
            node->V_ = {x_(i), x_(i + nPQ_)};
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
      for (int i = 0; i < Y_.size1(); ++i)
      {
         debug() << "\t\t" << std::setw(16) << row(Y_, i) << std::endl;
      }
   }
}
