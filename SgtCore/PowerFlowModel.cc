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

using namespace arma;

namespace Sgt
{
    PfBus::PfBus(const std::string& id, BusType type, const Phases& phases, const Col<Complex>& V,
            const Col<Complex>& YConst, const Col<Complex>& IConst, const Col<Complex>& Scg,
            double J) :
        id_(id),
        type_(type),
        phases_(phases),
        V_(V),
        YConst_(YConst),
        IConst_(IConst),
        Scg_(Scg),
        J_(J)
    {
        assert(V.size() == phases.size());
        assert(YConst.size() == phases.size());
        assert(IConst.size() == phases.size());
        assert(Scg.size() == phases.size());

        for (std::size_t i = 0; i < phases.size(); ++i)
        {
            nodeVec_.push_back(std::make_unique<PfNode>(*this, i));
        }
    }

    PfNode::PfNode(PfBus& bus, std::size_t phaseIdx) :
        bus_(&bus),
        phaseIdx_(phaseIdx)
    {
        // Empty.
    }

    PfBranch::PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
                       const Mat<Complex>& Y) :
        ids_{{id0, id1}},
        phases_{{phases0, phases1}},
        Y_(Y)
    {
        auto nTerm = phases0.size() + phases1.size();
        assert(Y.n_rows == nTerm);
        assert(Y.n_cols == nTerm);
    }

    PowerFlowModel::PowerFlowModel(size_t nBus, size_t nBranch)
    {
        sgtLogDebug() << "PowerFlowModel : constructor." << std::endl;
        busVec_.reserve(nBus);
        branchVec_.reserve(nBranch);
    }

    void PowerFlowModel::addBus(const std::string& id, BusType type, const Phases& phases,
            const arma::Col<Complex>& V, const arma::Col<Complex>& YConst, const arma::Col<Complex>& IConst,
            const arma::Col<Complex>& Scg, double J)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "PowerFlowModel : add bus " << id << std::endl;
        std::unique_ptr<PfBus> bus(new PfBus(id, type, phases, V, YConst, IConst, Scg, J));
        busVec_.push_back(bus.get());
        busMap_[id] = std::move(bus);
    }

    void PowerFlowModel::addBranch(const std::string& idBus0, const std::string& idBus1,
                                   const Phases& phases0, const Phases& phases1, const Mat<Complex>& Y)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "PowerFlowModel : addBranch " << idBus0 << " " << idBus1 << std::endl;
        branchVec_.push_back(std::make_unique<PfBranch>(idBus0, idBus1, phases0, phases1, Y));
    }

    void PowerFlowModel::validate()
    {
        sgtLogDebug() << "PowerFlowModel : validate." << std::endl;
        LogIndent indent;

        // Make Nodes:
        PfNodeVec PqNodes;
        PfNodeVec PvNodes;
        PfNodeVec SlNodes;

        for (auto bus : busVec_)
        {
            PfNodeVec* vec = nullptr;
            if (bus->type_ == BusType::PQ)
            {
                vec = &PqNodes;
            }
            else if (bus->type_ == BusType::PV)
            {
                vec = &PvNodes;
            }
            else if (bus->type_ == BusType::SL)
            {
                vec = &SlNodes;
            }
            else
            {
                sgtError("Unsupported bus type " << to_string(bus->type_) << ".");
            }
            for (const std::unique_ptr<PfNode>& node : bus->nodeVec_)
            {
                vec->push_back(node.get());
            }
        }

        nPq_ = PqNodes.size();
        nPv_ = PvNodes.size();
        nSl_ = SlNodes.size();

        // Insert nodes into ordered list of all nodes. Be careful of ordering!
        nodeVec_ = PfNodeVec();
        nodeVec_.reserve(nPq_ + nPv_ + nSl_);
        nodeVec_.insert(nodeVec_.end(), PqNodes.begin(), PqNodes.end());
        nodeVec_.insert(nodeVec_.end(), PvNodes.begin(), PvNodes.end());
        nodeVec_.insert(nodeVec_.end(), SlNodes.begin(), SlNodes.end());

        // Index all nodes:
        for (std::size_t i = 0; i < nodeVec_.size(); ++i)
        {
            nodeVec_[i]->idx_ = i;
        }

        auto nNode = nodeVec_.size();

        // Bus admittance matrix:
        SparseHelper<Complex> YHelper(nNode, nNode, true, true, true);

        // Branch admittances:
        for (const std::unique_ptr<PfBranch>& branch : branchVec_)
        {
            auto it0 = busMap_.find(branch->ids_[0]);
            sgtAssert(it0 != busMap_.end(), "BranchComp " << branch->ids_[0] << " " << branch->ids_[1] 
                    << " contains a non-existent bus " << branch->ids_[0] << ".");
            auto it1 = busMap_.find(branch->ids_[1]);
            sgtAssert(it1 != busMap_.end(), "BranchComp " << branch->ids_[0] << " " << branch->ids_[1] 
                    << " contains a non-existent bus " << branch->ids_[1] << ".");
            const PfBus* buses[] = {it0->second.get(), it1->second.get()};

            // There is one link per distinct pair of bus/phase pairs.
            for (std::size_t busIdxI = 0; busIdxI < 2; ++busIdxI)
            {
                for (std::size_t branchPhaseIdxI = 0; branchPhaseIdxI < branch->phases_[busIdxI].size(); 
                        ++branchPhaseIdxI)
                {
                    uword iTerm = busIdxI == 0 ? branchPhaseIdxI : branchPhaseIdxI + branch->phases_[0].size();
                    const PfBus* busI = buses[busIdxI];
                    auto busPhaseIdxI = busI->phases_.index(branch->phases_[busIdxI][branchPhaseIdxI]);
                    const PfNode* nodeI = busI->nodeVec_[busPhaseIdxI].get();
                    auto idxNodeI = nodeI->idx_;

                    // Only count each diagonal element in branch->Y_ once!
                    YHelper.insert(idxNodeI, idxNodeI, branch->Y_(iTerm, iTerm));

                    for (std::size_t busIdxK = 0; busIdxK < 2; ++busIdxK)
                    {
                        for (std::size_t branchPhaseIdxK = 0; branchPhaseIdxK < branch->phases_[busIdxK].size(); 
                                ++branchPhaseIdxK)
                        {
                            uword kTerm = busIdxK == 0 
                                ? branchPhaseIdxK 
                                : branchPhaseIdxK + branch->phases_[0].size();
                            const PfBus* busK = buses[busIdxK];
                            auto busPhaseIdxK = busK->phases_.index(branch->phases_[busIdxK][branchPhaseIdxK]);
                            const PfNode* nodeK = busK->nodeVec_[busPhaseIdxK].get();
                            auto idxNodeK = nodeK->idx_;

                            YHelper.insert(idxNodeI, idxNodeK, branch->Y_(iTerm, kTerm));
                            YHelper.insert(idxNodeK, idxNodeI, branch->Y_(kTerm, iTerm));
                        }
                    }
                }
            }
        } // Loop over branches.

        // Add shunt terms:
        for (uword i = 0; i < nNode; ++i)
        {
            YHelper.insert(i, i, nodeVec_[i]->YConst());
        }

        Y_ = YHelper.get();

        sgtLogDebug() << "Y_.nnz() = " << Y_.n_nonzero << std::endl;

        sgtLogDebug() << "PowerFlowModel : validate complete." << std::endl;
        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            print();
        }
    }

    void PowerFlowModel::print()
    {
        sgtLogDebug() << "PowerFlowModel::print()" << std::endl;
        LogIndent indent;
        sgtLogDebug() << "Nodes:" << std::endl;
        {
            LogIndent indent;
            for (const PfNode* nd : nodeVec_)
            {
                sgtLogDebug() << "Node:" << std::endl;
                {
                    LogIndent indent;
                    sgtLogDebug() << "Id     : " << nd->bus_->id_ << std::endl;
                    sgtLogDebug() << "Type   : " << nd->bus_->type_ << std::endl;
                    sgtLogDebug() << "Phase  : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
                    sgtLogDebug() << "V      : " << nd->V() << std::endl;
                    sgtLogDebug() << "YConst : " << nd->YConst() << std::endl;
                    sgtLogDebug() << "IConst : " << nd->IConst() << std::endl;
                    sgtLogDebug() << "Scg    : " << nd->Scg() << std::endl;
                }
            }
        }
        sgtLogDebug() << "Branches:" << std::endl;
        {
            LogIndent indent;
            for (const std::unique_ptr<PfBranch>& branch : branchVec_)
            {
                sgtLogDebug() << "Branch:" << std::endl;
                {
                    LogIndent indent;
                    sgtLogDebug() << "Buses : " << branch->ids_[0] << ", " << branch->ids_[1] << std::endl;
                    sgtLogDebug() << "Phases : " << branch->phases_[0] << ", " << branch->phases_[1] << std::endl;
                    sgtLogDebug() << "Y      :" << std::endl;
                    {
                        LogIndent indent;
                        for (uword i = 0; i < branch->Y_.n_rows; ++i)
                        {
                            sgtLogDebug() << std::setprecision(14) << std::setw(18) << branch->Y_.row(i) << std::endl;
                        }
                    }
                }
            }
        }
    }

    Col<Complex> PowerFlowModel::V() const
    {
        auto it = nodeVec_.begin();
        return Col<Complex>(nodeVec_.size()).imbue([&](){return (**(it++)).V();});
    }
    void PowerFlowModel::setV(const Col<Complex>& V) const
    {
        for (uword i = 0; i < V.size(); ++i)
        {
            nodeVec_[i]->setV(V[i]);
        }
    }

    Col<Complex> PowerFlowModel::Scg() const
    {
        auto it = nodeVec_.begin();
        return Col<Complex>(nodeVec_.size()).imbue([&](){return (**(it++)).Scg();});
    }

    void PowerFlowModel::setScg(const Col<Complex>& Scg) const
    {
        for (uword i = 0; i < Scg.size(); ++i)
        {
            nodeVec_[i]->setScg(Scg[i]);
        }
    }

    Col<Complex> PowerFlowModel::IConst() const
    {
        auto it = nodeVec_.begin();
        return Col<Complex>(nodeVec_.size()).imbue([&](){return (**(it++)).IConst();});
    }
    void PowerFlowModel::setIConst(const Col<Complex>& IConst) const
    {
        for (uword i = 0; i < IConst.size(); ++i)
        {
            nodeVec_[i]->setIConst(IConst[i]);
        }
    }
}
