// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PowerFlowModel.h"

#include "SparseHelper.h"

#include <iostream>
#include <sstream>

namespace Sgt
{
    PfBus::PfBus(const std::string& id, BusType type, const Phases& phases,
                 const arma::Col<Complex>& YConst, const arma::Col<Complex>& IConst, const arma::Col<Complex>& SConst,
                 double J, const arma::Col<Complex>& V, const arma::Col<Complex>& S) :
        id_(id),
        type_(type),
        phases_(phases),
        YConst_(YConst),
        IConst_(IConst),
        SConst_(SConst),
        J_(J),
        V_(V),
        S_(S)
    {
        assert(YConst.size() == phases.size());
        assert(IConst.size() == phases.size());
        assert(SConst.size() == phases.size());
        assert(V.size() == phases.size());
        assert(S.size() == phases.size());

        for (std::size_t i = 0; i < phases.size(); ++i)
        {
            nodes_.push_back(std::unique_ptr<PfNode>(new PfNode(*this, i)));
        }
    }

    PfNode::PfNode(PfBus& bus, int phaseIdx) :
        bus_(&bus),
        phaseIdx_(phaseIdx),
        YConst_(bus.YConst_(phaseIdx)),
        IConst_(bus.IConst_(phaseIdx)),
        SConst_(bus.SConst_(phaseIdx)),
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
        auto nTerm = 2 * nPhase_;
        assert(Y.n_rows == nTerm);
        assert(Y.n_cols == nTerm);
    }

    void PowerFlowModel::addBus(const std::string& id, BusType type, const Phases& phases,
            const arma::Col<Complex>& YConst, const arma::Col<Complex>& IConst, const arma::Col<Complex>& SConst,
            double J, const arma::Col<Complex>& V, const arma::Col<Complex>& S)
    {
        SGT_DEBUG(Log().debug() << "PowerFlowModel : add bus " << id << std::endl);
        busses_[id].reset(new PfBus(id, type, phases, YConst, IConst, SConst, J, V, S));
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
                Log().error() << "Unsupported bus type " << to_string(bus.type_) << std::endl;
                error();
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
        for (std::size_t i = 0; i < nodes_.size(); ++i)
        {
            nodes_[i]->idx_ = i;
        }

        auto nNode = nodes_.size();

        // Bus admittance matrix:
        SparseHelper<Complex> YHelper(nNode, nNode, true, true, true);


        // Branch admittances:
        for (const std::unique_ptr<PfBranch>& branch : branches_)
        {
            auto it0 = busses_.find(branch->ids_[0]);
            if (it0 == busses_.end())
            {
                Log().error() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
                              << " contains a non-existent bus " << branch->ids_[0] << std::endl;
                error();
            }
            auto it1 = busses_.find(branch->ids_[1]);
            if (it1 == busses_.end())
            {
                Log().error() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
                              << " contains a non-existent bus " << branch->ids_[1] << std::endl;
                error();
            }
            const PfBus* busses[] = {it0->second.get(), it1->second.get()};
            auto nTerm = 2 * branch->nPhase_;

            // There is one link per distinct pair of bus/phase pairs.
            for (arma::uword i = 0; i < nTerm; ++i)
            {
                auto busIdxI = i / branch->nPhase_; // 0 or 1
                auto branchPhaseIdxI = i % branch->nPhase_; // 0 to nPhase of branch.
                const PfBus* busI = busses[busIdxI];
                auto busPhaseIdxI = busI->phases_.phaseIndex(branch->phases_[busIdxI][branchPhaseIdxI]);
                const PfNode* nodeI = busI->nodes_[busPhaseIdxI].get();
                auto idxNodeI = nodeI->idx_;

                // Only count each diagonal element in branch->Y_ once!
                YHelper.insert(idxNodeI, idxNodeI, branch->Y_(i, i));

                for (arma::uword k = i + 1; k < nTerm; ++k)
                {
                    int busIdxK = k / branch->nPhase_; // 0 or 1
                    int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
                    const PfBus* busK = busses[busIdxK];
                    int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
                    const PfNode* nodeK = busK->nodes_[busPhaseIdxK].get();
                    int idxNodeK = nodeK->idx_;

                    YHelper.insert(idxNodeI, idxNodeK, branch->Y_(i, k));
                    YHelper.insert(idxNodeK, idxNodeI, branch->Y_(k, i));
                }
            }
        } // Loop over branches.

        // Add shunt terms:
        for (arma::uword i = 0; i < nNode; ++i)
        {
            YHelper.insert(i, i, nodes_[i]->YConst_);
        }

        Y_ = YHelper.get();

        SGT_DEBUG(Log().debug() << "Y_.nnz() = " << Y_.nnz() << std::endl);

        // Vector quantities of problem:
        V_.set_size(nNode);
        S_.set_size(nNode);
        IConst_.set_size(nNode);

        for (arma::uword i = 0; i < nNode; ++i)
        {
            const PfNode& node = *nodes_[i];
            V_(i) = node.V_;
            S_(i) = node.S_;
            IConst_(i) = node.IConst_;
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
                    Log().debug() << "Id     : " << nd->bus_->id_ << std::endl;
                    Log().debug() << "Type   : " << nd->bus_->type_ << std::endl;
                    Log().debug() << "Phase  : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
                    Log().debug() << "V      : " << nd->V_ << std::endl;
                    Log().debug() << "S      : " << nd->S_ << std::endl;
                    Log().debug() << "YConst : " << nd->YConst_ << std::endl;
                    Log().debug() << "IConst : " << nd->IConst_ << std::endl;
                    Log().debug() << "SConst : " << nd->SConst_ << std::endl;
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
                        for (arma::uword i = 0; i < branch->Y_.n_rows; ++i)
                        {
                            Log().debug() << std::setprecision(14) << std::setw(18) << branch->Y_.row(i) << std::endl;
                        }
                    }
                }
            }
        }
    }
}
