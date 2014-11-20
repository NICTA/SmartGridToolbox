#include "PowerFlowModel.h"

#include <iostream>
#include <sstream>

namespace SmartGridToolbox
{
   PfBus::PfBus(const std::string& id, BusType type, const Phases& phases,
         const arma::Col<Complex>& YZip, const arma::Col<Complex>& IZip, const arma::Col<Complex>& SZip,
         double J, const arma::Col<Complex>& V, const arma::Col<Complex>& S) :
      id_(id),
      type_(type),
      phases_(phases),
      YZip_(YZip),
      IZip_(IZip),
      SZip_(SZip),
      J_(J),
      V_(V),
      S_(S)
   {
      assert(YZip.size() == phases.size());
      assert(IZip.size() == phases.size());
      assert(SZip.size() == phases.size());
      assert(V.size() == phases.size());
      assert(S.size() == phases.size());

      for (int i = 0; i < phases.size(); ++i)
      {
         nodes_.push_back(std::unique_ptr<PfNode>(new PfNode(*this, i)));
      }
   }

   PfNode::PfNode(PfBus& bus, int phaseIdx) :
      bus_(&bus),
      phaseIdx_(phaseIdx),
      YZip_(bus.YZip_(phaseIdx)),
      IZip_(bus.IZip_(phaseIdx)),
      SZip_(bus.SZip_(phaseIdx)),
      V_(bus.V_(phaseIdx)),
      S_(bus.S_(phaseIdx)),
      idx_(-1)
   {
      // Empty.
   }

   PfBranch::PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
                      const arma::Mat<Complex>& Y) :
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

   void PowerFlowModel::addBus(const std::string& id, BusType type, const Phases& phases,
         const arma::Col<Complex>& YZip, const arma::Col<Complex>& IZip, const arma::Col<Complex>& SZip,
         double J, const arma::Col<Complex>& V, const arma::Col<Complex>& S)
   {
      SGT_DEBUG(Log().debug() << "PowerFlowModel : add bus " << id << std::endl);
      busses_[id].reset(new PfBus(id, type, phases, YZip, IZip, SZip, J, V, S));
   }

   void PowerFlowModel::addBranch(const std::string& idBus0, const std::string& idBus1,
         const Phases& phases0, const Phases& phases1, const arma::Mat<Complex>& Y)
   {
      SGT_DEBUG(Log().debug() << "PowerFlowModel : addBranch " << idBus0 << " " << idBus1 << std::endl);
      branches_.push_back(std::unique_ptr<PfBranch>(new PfBranch(idBus0, idBus1, phases0, phases1, Y)));
   }

   void PowerFlowModel::reset()
   {
      SGT_DEBUG(Log().debug() << "PowerFlowModel : reset." << std::endl);
      busses_ = PfBusMap();
      branches_ = PfBranchVec();
   }

   void PowerFlowModel::validate()
   {
      SGT_DEBUG(Log().debug() << "PowerFlowModel : validate." << std::endl);

      // Make Nodes:
      PfNodeVec PqNodes = PfNodeVec();
      PfNodeVec PvNodes = PfNodeVec();
      PfNodeVec SlNodes = PfNodeVec();

      for (auto& busPair : busses_)
      {
         PfBus& bus = *busPair.second;
         PfNodeVec* vec = nullptr;
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
            Log().fatal() << "Unsupported bus type " << to_string(bus.type_) << std::endl;
         }
         for (const std::unique_ptr<PfNode>& node : bus.nodes_)
         {
            vec->push_back(node.get());
         }
      }

      nSl_ = SlNodes.size();
      nPq_ = PqNodes.size();
      nPv_ = PvNodes.size();

      // Insert nodes into ordered list of all nodes. Be careful of ordering!
      nodes_ = PfNodeVec();
      nodes_.reserve(nPq_ + nPv_ + nSl_);
      nodes_.insert(nodes_.end(), SlNodes.begin(), SlNodes.end());
      nodes_.insert(nodes_.end(), PqNodes.begin(), PqNodes.end());
      nodes_.insert(nodes_.end(), PvNodes.begin(), PvNodes.end());

      // Index all nodes:
      for (int i = 0; i < nodes_.size(); ++i)
      {
         nodes_[i]->idx_ = i;
      }

      int nNode = nodes_.size();

      // Bus admittance matrix:
      Y_.resize(nNode, nNode, false);

      // Branch admittances:
      for (const std::unique_ptr<PfBranch>& branch : branches_)
      {
         auto it0 = busses_.find(branch->ids_[0]);
         if (it0 == busses_.end())
         {
            Log().fatal() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
               << " contains a non-existent bus " << branch->ids_[0] << std::endl;
         }
         auto it1 = busses_.find(branch->ids_[1]);
         if (it1 == busses_.end())
         {
            Log().fatal() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
               << " contains a non-existent bus " << branch->ids_[1] << std::endl;
         }
         const PfBus* busses[] = {it0->second.get(), it1->second.get()};
         int nTerm = 2 * branch->nPhase_;

         // There is one link per distinct pair of bus/phase pairs.
         for (int i = 0; i < nTerm; ++i)
         {
            int busIdxI = i / branch->nPhase_; // 0 or 1
            int branchPhaseIdxI = i % branch->nPhase_; // 0 to nPhase of branch.
            const PfBus* busI = busses[busIdxI];
            int busPhaseIdxI = busI->phases_.phaseIndex(branch->phases_[busIdxI][branchPhaseIdxI]);
            const PfNode* nodeI = busI->nodes_[busPhaseIdxI].get();
            int idxNodeI = nodeI->idx_;

            // Only count each diagonal element in branch->Y_ once!
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i);

            for (int k = i + 1; k < nTerm; ++k)
            {
               int busIdxK = k / branch->nPhase_; // 0 or 1
               int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
               const PfBus* busK = busses[busIdxK];
               int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
               const PfNode* nodeK = busK->nodes_[busPhaseIdxK].get();
               int idxNodeK = nodeK->idx_;

               Y_(idxNodeI, idxNodeK) += branch->Y_(i, k);
               Y_(idxNodeK, idxNodeI) += branch->Y_(k, i);
            }
         }
      } // Loop over branches.

      // Add shunt terms:
      for (int i = 0; i < nNode; ++i)
      {
         Y_(i, i) += nodes_[i]->YZip_;
      }

      SGT_DEBUG(Log().debug() << "Y_.nnz() = " << Y_.nnz() << std::endl);

      // Vector quantities of problem:
      V_.resize(nNode, false);
      S_.resize(nNode, false);
      IZip_.resize(nNode, false);

      for (int i = 0; i < nNode; ++i)
      {
         const PfNode& node = *nodes_[i];
         V_(i) = node.V_;
         S_(i) = node.S_;
         IZip_(i) = node.IZip_;
      }

      SGT_DEBUG(Log().debug() << "PowerFlowModel : validate complete." << std::endl);
      SGT_DEBUG(print());
   }

   void PowerFlowModel::print()
   {
      Log().debug() << "PowerFlowModel::print()" << std::endl;
      LogIndent _;
      Log().debug() << "Nodes:" << std::endl;
      {
         LogIndent _;
         for (const PfNode* nd : nodes_)
         {
            Log().debug() << "Node:" << std::endl;
            {
               LogIndent _;
               Log().debug() << "Id    : " << nd->bus_->id_ << std::endl;
               Log().debug() << "Type  : " << nd->bus_->type_ << std::endl;
               Log().debug() << "Phase : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
               Log().debug() << "V     : " << nd->V_ << std::endl;
               Log().debug() << "S     : " << nd->S_ << std::endl;
               Log().debug() << "YZip    : " << nd->YZip_ << std::endl;
               Log().debug() << "IZip    : " << nd->IZip_ << std::endl;
               Log().debug() << "SZip    : " << nd->SZip_ << std::endl;
            }
         }
      }
      Log().debug() << "Branches:" << std::endl;
      {
         LogIndent _;
         for (const std::unique_ptr<PfBranch>& branch : branches_)
         {
            Log().debug() << "Branch:" << std::endl;
            {
               LogIndent _;
               Log().debug() << "Busses : " << branch->ids_[0] << ", " << branch->ids_[1] << std::endl;
               Log().debug() << "Phases : " << branch->phases_[0] << ", " << branch->phases_[1] << std::endl;
               Log().debug() << "Y      :" << std::endl;
               {
                  LogIndent _;
                  for (int i = 0; i < branch->Y_.size1(); ++i)
                  {
                     Log().debug() << std::setprecision(14) << std::setw(18) << row(branch->Y_, i) << std::endl;
                  }
               }
            }
         }
      }
   }
}
