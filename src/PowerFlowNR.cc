#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   BusNR::BusNR(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
                const UblasVector<Complex> & Ys, const UblasVector<Complex> & Ic, const UblasVector<Complex> & Sc) :
      id_(id),
      type_(type),
      phases_(phases),
      V_(V),
      S_(Sc),
      Ys_(Ys),
      Ic_(Ic),
      Sc_(Sc)
   {
      assert(V.size() == phases.size());
      assert(Ys.size() == phases.size());
      assert(Ic.size() == phases.size());
      assert(Sc.size() == phases.size());

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
      Ys_(bus.Ys_(phaseIdx)),
      Ic_(bus.Ic_(phaseIdx)),
      Sc_(bus.Sc_(phaseIdx)),
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
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i) + nodeI->Ys_;

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
      for (int i = 0; i < nNode(); ++i)
      {
         const NodeNR & node = *nodes_[i];
         P(i) = node.S_.real();
         Q(i) = node.S_.imag();
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   void PowerFlowNR::initJC(UblasCMatrix<double> & JC) const
   {
      const auto G = real(Y_);
      const auto B = imag(Y_);

      project(JC, selIrPQFromf(), selVrPQFromx()) = -project(G, selPQFromAll(), selPQFromAll());
      project(JC, selIrPQFromf(), selViPQFromx()) =  project(B, selPQFromAll(), selPQFromAll());
      project(JC, selIiPQFromf(), selVrPQFromx()) = -project(B, selPQFromAll(), selPQFromAll());
      project(JC, selIiPQFromf(), selViPQFromx()) = -project(G, selPQFromAll(), selPQFromAll());

      project(JC, selIrPVFromf(), selVrPQFromx()) = -project(G, selPVFromAll(), selPQFromAll());
      project(JC, selIrPVFromf(), selViPQFromx()) =  project(B, selPVFromAll(), selPQFromAll());
      project(JC, selIiPVFromf(), selVrPQFromx()) = -project(B, selPVFromAll(), selPQFromAll());
      project(JC, selIiPVFromf(), selViPQFromx()) = -project(G, selPVFromAll(), selPQFromAll());

      project(JC, selIrPQFromf(), selViPVFromx()) =  project(B, selPQFromAll(), selPVFromAll());
      project(JC, selIiPQFromf(), selViPVFromx()) = -project(G, selPQFromAll(), selPVFromAll());

      project(JC, selIrPVFromf(), selViPVFromx()) =  project(B, selPVFromAll(), selPVFromAll());
      project(JC, selIiPVFromf(), selViPVFromx()) = -project(G, selPVFromAll(), selPVFromAll());
   }

   void PowerFlowNR::updatef(UblasVector<double> & f, 
                             const UblasVector<double> & Vr, const UblasVector<double> & Vi,
                             const UblasVector<double> & P, const UblasVector<double> & Q) const
   {
      const auto G = real(Y_);
      const auto B = imag(Y_);

      const auto GRng = project(G, selPQPVFromAll(), selAllFromAll());
      const auto BRng = project(B, selPQPVFromAll(), selAllFromAll());

      const auto VrPQPV = project(Vr, selPQPVFromAll());
      const auto ViPQPV = project(Vi, selPQPVFromAll());
      
      const auto PPQPV = project(P, selPQPVFromAll());
      const auto QPQPV = project(Q, selPQPVFromAll());

      UblasVector<double> M2PQPV = element_prod(VrPQPV, VrPQPV) + element_prod(ViPQPV, ViPQPV);

      auto Ir = element_div(element_prod(VrPQPV, PPQPV) + element_prod(ViPQPV, QPQPV), M2PQPV)
                            + project(real(Ic_), selPQPVFromAll())
                            - prod(GRng, Vr) + prod(BRng, Vi);
      auto Ii = element_div(element_prod(ViPQPV, PPQPV) - element_prod(VrPQPV, QPQPV), M2PQPV)
                            + project(imag(Ic_), selPQPVFromAll())
                            - prod(GRng, Vi) - prod(BRng, Vr);
      project(f, selIrPQFromf()) = project(Ir, selPQFromPQPV());
      project(f, selIiPQFromf()) = project(Ii, selPQFromPQPV());
      project(f, selIrPVFromf()) = project(Ir, selPVFromPQPV());
      project(f, selIiPVFromf()) = project(Ii, selPVFromPQPV());
   }

   void PowerFlowNR::updateJ(UblasCMatrix<double> & J, const UblasCMatrix<double> & JC,
                             const UblasVector<double> Vr, const UblasVector<double> Vi,
                             const UblasVector<double> P, const UblasVector<double> Q) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in JC at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.
      // Thus we need to do (PQ,PQ) diagonal and (*, PV) columns.
    
      const auto G = real(Y_);
      const auto B = imag(Y_);

      auto VrPQ = project(Vr, selPQFromAll());
      auto ViPQ = project(Vi, selPQFromAll());
      auto PPQ = project(P, selPQFromAll());
      auto QPQ = project(Q, selPQFromAll());

      auto VrPV = project(Vr, selPVFromAll());
      auto ViPV = project(Vi, selPVFromAll());
      auto PPV = project(P, selPVFromAll());
      auto QPV = project(Q, selPVFromAll());

      // (PQ, PQ):
      auto JPQIrPQVr = project(J, selIrPQFromf(), selVrPQFromx());
      auto JPQIrPQVi = project(J, selIrPQFromf(), selViPQFromx());
      auto JPQIiPQVr = project(J, selIiPQFromf(), selVrPQFromx());
      auto JPQIiPQVi = project(J, selIiPQFromf(), selViPQFromx());

      auto JCPQIrPQVr = project(JC, selIrPQFromf(), selVrPQFromx());
      auto JCPQIrPQVi = project(JC, selIrPQFromf(), selViPQFromx());
      auto JCPQIiPQVr = project(JC, selIiPQFromf(), selVrPQFromx());
      auto JCPQIiPQVi = project(JC, selIiPQFromf(), selViPQFromx());
      
      // (PQ, PV)
      auto JPQIrPVQ = project(J, selIrPQFromf(), selQPVFromx());
      auto JPQIrPVVi = project(J, selIrPQFromf(), selViPVFromx());
      auto JPQIiPVQ = project(J, selIiPQFromf(), selQPVFromx());
      auto JPQIiPVVi = project(J, selIiPQFromf(), selViPVFromx());

      auto JCPQIrPVQ = project(JC, selIrPQFromf(), selQPVFromx());
      auto JCPQIrPVVi = project(JC, selIrPQFromf(), selViPVFromx());
      auto JCPQIiPVQ = project(JC, selIiPQFromf(), selQPVFromx());
      auto JCPQIiPVVi = project(JC, selIiPQFromf(), selViPVFromx());

      auto GPQPV = project(G, selPQFromAll(), selPVFromAll());
      auto BPQPV = project(G, selPQFromAll(), selPVFromAll());

      // (PV, PQ):
      auto JPVIrPQIr = project(J, selIrPVFromf(), selVrPQFromx());
      auto JPVIrPQIi = project(J, selIrPVFromf(), selViPQFromx());
      auto JPVIiPQIr = project(J, selIiPVFromf(), selVrPQFromx());
      auto JPVIiPQIi = project(J, selIiPVFromf(), selViPQFromx());

      auto JCPVIrPQIr = project(JC, selIrPVFromf(), selVrPQFromx());
      auto JCPVIrPQIi = project(JC, selIrPVFromf(), selViPQFromx());
      auto JCPVIiPQIr = project(JC, selIiPVFromf(), selVrPQFromx());
      auto JCPVIiPQIi = project(JC, selIiPVFromf(), selViPQFromx());

      // (PV, PV):
      auto JPVIrPVQ = project(J, selIrPVFromf(), selQPVFromx());
      auto JPVIrPVVi = project(J, selIrPVFromf(), selViPVFromx());
      auto JPVIiPVQ = project(J, selIiPVFromf(), selQPVFromx());
      auto JPVIiPVVi = project(J, selIiPVFromf(), selViPVFromx());

      auto JCPVIrPVQ = project(JC, selIrPVFromf(), selQPVFromx());
      auto JCPVIrPVVi = project(JC, selIrPVFromf(), selViPVFromx());
      auto JCPVIiPVQ = project(JC, selIiPVFromf(), selQPVFromx());
      auto JCPVIiPVVi = project(JC, selIiPVFromf(), selViPVFromx());

      auto GPVPV = project(G, selPVFromAll(), selPVFromAll());
      auto BPVPV = project(G, selPVFromAll(), selPVFromAll());

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

      // (PV, PV) block diagonal:
      for (int i = 0; i < nPV_; ++i)
      {
         double M2 = VrPV(i) * VrPV(i);

         JPVIrPVQ(i, i) += ViPV(i) / M2;
         JPVIiPVQ(i, i) -= VrPV(i) / M2;
         JPVIrPVVi(i, i) += (QPV(i) - ViPV(i) * PPV(i) / VrPV(i)) / M2;
         JPVIiPVVi(i, i) += (PPV(i) + ViPV(i) * QPV(i) / VrPV(i)) / M2;
      }
   }

   bool PowerFlowNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : solve." << std::endl);

      const double tol = 1e-10;
      const int maxiter = 20;

      UblasVector<double> Vr(nNode());
      UblasVector<double> Vi(nNode());
      initV(Vr, Vi);
     
      auto M2PV = element_prod(project(Vr, selPVFromAll()), project(Vr, selPVFromAll()))
                + element_prod(project(Vi, selPVFromAll()), project(Vi, selPVFromAll()));

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
         double err = sqrt(*std::max_element(f2.begin(), f2.end()));
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
            debug() << "\tBefore KLUSolve: Vr  = " << std::setw(8) << Vr << std::endl;
            debug() << "\tBefore KLUSolve: Vi  = " << std::setw(8) << Vi << std::endl;
            debug() << "\tBefore KLUSolve: M^2 = " << std::setw(8) 
                    << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl;
            debug() << "\tBefore KLUSolve: P   = " << std::setw(8) << P << std::endl;
            debug() << "\tBefore KLUSolve: Q   = " << std::setw(8) << Q << std::endl;
            debug() << "\tBefore KLUSolve: f   = " << std::setw(8) << f << std::endl;
            debug() << "\tBefore KLUSolve: J   = " << std::endl;
            for (int i = 0; i < nVar(); ++i)
            {
               debug() << "\t\t" << std::setw(8) << row(J, i) << std::endl;
            }
         );

         bool ok = KLUSolve(J, -f, rhs);
         SGT_DEBUG(debug() << "\tAfter KLUSolve: ok = " << ok << ", x = " << std::setw(8) << rhs << std::endl);
         if (!ok)
         {
            error() << "KLUSolve failed." << std::endl;
            abort();
         }

         // Update the current values of V from rhs:
         project(Vr, selPQFromAll()) += project(rhs, selVrPQFromx());
         project(Vi, selPQFromAll()) += project(rhs, selViPQFromx());

         project(Vi, selPVFromAll()) += project(rhs, selViPVFromx());
         project(Vr, selPVFromAll()) += 
            element_div(M2PV - element_prod(project(Vr, selPVFromAll()), project(Vr, selPVFromAll()))
                             - element_prod(project(Vi, selPVFromAll()), project(Vi, selPVFromAll())),
                        2 * project(Vr, selPVFromAll()))
            - element_div(element_prod(project(Vi, selPVFromAll()), project(rhs, selViPVFromx())),
                          project(Vr, selPVFromAll()));
         project(Q, selPVFromAll()) += project(rhs, selQPVFromx());

         project(P, selSLFromAll())

         SGT_DEBUG(debug() << "\tUpdated Vr  = " << std::setw(8) << Vr << std::endl);
         SGT_DEBUG(debug() << "\tUpdated Vi  = " << std::setw(8) << Vi << std::endl);
         SGT_DEBUG(debug() << "\tUpdated M^2 = " << std::setw(8) 
                           << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl);
         SGT_DEBUG(debug() << "\tUpdated P   = " << std::setw(8) << P << std::endl);
         SGT_DEBUG(debug() << "\tUpdated Q   = " << std::setw(8) << Q << std::endl);
      }
      if (wasSuccessful)
      {
         UblasVector<Complex> V(nNode());
         UblasVector<Complex> S(nNode());

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
         debug() << "\t\t\tY     : " << nd->Ys_ << std::endl;
         debug() << "\t\t\tI     : " << nd->Ic_ << std::endl;
         debug() << "\t\t\tS     : " << nd->Sc_ << std::endl;
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
         debug() << "\t\t\t\t" << std::setw(16) << row(Y_, i) << std::endl;
      }
   }
}
