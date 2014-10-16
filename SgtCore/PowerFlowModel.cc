#include "PowerFlowModel.h"

#include <iostream>
#include <sstream>

namespace SmartGridToolbox
{
   PfBus::PfBus(const std::string& id, BusType type, const Phases& phases,
         const ublas::vector<Complex>& Ys, const ublas::vector<Complex>& Ic,
         const ublas::vector<Complex>& V, const ublas::vector<Complex>& S) :
      id_(id),
      type_(type),
      phases_(phases),
      Ys_(Ys),
      Ic_(Ic),
      V_(V),
      S_(S)
   {
      assert(V.size() == phases.size());
      assert(Ys.size() == phases.size());
      assert(Ic.size() == phases.size());
      assert(S.size() == phases.size());

      for (int i = 0; i < phases.size(); ++i)
      {
         nodes_.push_back(std::unique_ptr<PfNode>(new PfNode(*this, i)));
      }
   }

   PfNode::PfNode(PfBus& bus, int phaseIdx) :
      bus_(&bus),
      phaseIdx_(phaseIdx),
      Ys_(bus.Ys_(phaseIdx)),
      Ic_(bus.Ic_(phaseIdx)),
      V_(bus.V_(phaseIdx)),
      S_(bus.S_(phaseIdx)),
      idx_(-1)
   {
      // Empty.
   }

   PfBranch::PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
                      const ublas::matrix<Complex>& Y) :
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
         const ublas::vector<Complex>& V, const ublas::vector<Complex>& Y, const ublas::vector<Complex>& I,
         const ublas::vector<Complex>& S)
   {
      SGT_DEBUG(Log().debug() << "PowerFlowModel : add bus " << id << std::endl);
      busses_[id].reset(new PfBus(id, type, phases, V, Y, I, S));
   }

   void PowerFlowModel::addBranch(const std::string& idBus0, const std::string& idBus1,
         const Phases& phases0, const Phases& phases1, const ublas::matrix<Complex>& Y)
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
            Log().fatal() << "Unsupported bus type " << busType2Str(bus.type_) << std::endl;
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
   }

   void PowerFlowModel::printModel()
   {
      Log().debug() << "PowerFlowNr::printModel()" << std::endl;
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
               Log().debug() << "Ys    : " << nd->Ys_ << std::endl;
               Log().debug() << "Ic    : " << nd->Ic_ << std::endl;
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
