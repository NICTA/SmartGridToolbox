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
            const Mat<Complex>& YConst, const Mat<Complex>& IConst, const Mat<Complex>& SConst,
            const Col<Complex>& SGen, double JGen) :
        id_(id),
        type_(type),
        phases_(phases),
        V_(V),
        YConst_(YConst),
        IConst_(IConst),
        SConst_(SConst),
        SGen_(SGen),
        JGen_(JGen)
    {
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
                       const Mat<Complex>& YConst) :
        ids_{{id0, id1}},
        phases_{{phases0, phases1}},
        YConst_(YConst)
    {
        std::size_t nTerm = phases0.size() + phases1.size();
        assert(YConst.n_rows == nTerm);
        assert(YConst.n_cols == nTerm);
    }

    PowerFlowModel::PowerFlowModel(size_t nBus, size_t nBranch)
    {
        sgtLogDebug() << "PowerFlowModel : constructor." << std::endl;
        busVec_.reserve(nBus);
        branchVec_.reserve(nBranch);
    }

    void PowerFlowModel::addBus(const std::string& id, BusType type, const Phases& phases,
            const arma::Col<Complex>& V, const arma::Mat<Complex>& YConst, const arma::Mat<Complex>& IConst,
            const arma::Mat<Complex>& SConst, const arma::Col<Complex> SGen, double JGen)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "PowerFlowModel : add bus " << id << std::endl;
        std::unique_ptr<PfBus> bus(new PfBus(id, type, phases, V, YConst, IConst, SConst, SGen, JGen));
        busVec_.push_back(bus.get());
        busMap_[id] = std::move(bus);
    }

    void PowerFlowModel::addBranch(const std::string& idBus0, const std::string& idBus1,
                                   const Phases& phases0, const Phases& phases1, const Mat<Complex>& YConst)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "PowerFlowModel : addBranch " << idBus0 << " " << idBus1 << std::endl;
        branchVec_.push_back(std::make_unique<PfBranch>(idBus0, idBus1, phases0, phases1, YConst));
    }

    void PowerFlowModel::validate()
    {
        sgtLogDebug() << "PowerFlowModel : validate." << std::endl;
        sgtLogIndent();

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

        std::size_t nNode = nodeVec_.size();

        SparseHelper<Complex> YConstHelper(nNode, nNode, true, true, true);
        SparseHelper<Complex> IConstHelper(nNode, nNode, true, true, true);
        SparseHelper<Complex> SConstHelper(nNode, nNode, true, true, true);

        for (auto& bus : busVec_)
        {
            for (std::size_t iPh = 0; iPh < bus->phases_.size(); ++iPh)
            {
                const auto& ndI = bus->nodeVec_[iPh];
                auto iNd = ndI->idx_;
                    
                YConstHelper.insert(iNd, iNd, bus->YConst_(iPh, iPh)); // NOTE: filters out zeros.
                IConstHelper.insert(iNd, iNd, bus->IConst_(iPh, iPh)); // NOTE: filters out zeros.
                SConstHelper.insert(iNd, iNd, bus->SConst_(iPh, iPh)); // NOTE: filters out zeros.

                for (std::size_t jPh = iPh + 1; jPh < bus->phases_.size(); ++jPh)
                {
                    const auto& ndJ = bus->nodeVec_[jPh];
                    auto jNd = ndJ->idx_;
                    YConstHelper.insert(iNd, jNd, bus->YConst_(iPh, jPh)); // NOTE: filters out zeros.
                    IConstHelper.insert(iNd, jNd, bus->IConst_(iPh, jPh)); // NOTE: filters out zeros.
                    SConstHelper.insert(iNd, jNd, bus->SConst_(iPh, jPh)); // NOTE: filters out zeros.
                    YConstHelper.insert(jNd, iNd, bus->YConst_(iPh, jPh)); // NOTE: filters out zeros.
                    IConstHelper.insert(jNd, iNd, bus->IConst_(iPh, jPh)); // NOTE: filters out zeros.
                    SConstHelper.insert(jNd, iNd, bus->SConst_(iPh, jPh)); // NOTE: filters out zeros.
                }
            }
        }

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
            std::size_t nTerm = branch->phases_[0].size() + branch->phases_[1].size();

            // There is one link per distinct pair of bus/phase pairs.
            for (uword i = 0; i < nTerm; ++i)
            {
                bool firstBusI = (i < branch->phases_[0].size());
                std::size_t busIdxI = firstBusI ? 0 : 1;
                std::size_t branchPhaseIdxI = firstBusI ? i : i - branch->phases_[0].size();

                const PfBus* busI = buses[busIdxI];
                std::size_t busPhaseIdxI = busI->phases_.index(branch->phases_[busIdxI][branchPhaseIdxI]);
                const PfNode* nodeI = busI->nodeVec_[busPhaseIdxI].get();
                std::size_t idxNodeI = nodeI->idx_;

                // Only count each diagonal element in branch->YConst_ once!
                YConstHelper.insert(idxNodeI, idxNodeI, branch->YConst_(i, i));

                for (uword k = i + 1; k < nTerm; ++k)
                {
                    bool firstBusK = (k < branch->phases_[0].size());
                    std::size_t busIdxK = firstBusK ? 0 : 1;
                    std::size_t branchPhaseIdxK = firstBusK ? k : k - branch->phases_[0].size();

                    const PfBus* busK = buses[busIdxK];
                    std::size_t busPhaseIdxK = busK->phases_.index(branch->phases_[busIdxK][branchPhaseIdxK]);
                    const PfNode* nodeK = busK->nodeVec_[busPhaseIdxK].get();
                    std::size_t idxNodeK = nodeK->idx_;

                    YConstHelper.insert(idxNodeI, idxNodeK, branch->YConst_(i, k));
                    YConstHelper.insert(idxNodeK, idxNodeI, branch->YConst_(k, i));
                }
            }
        } // Loop over branches.

        YConst_ = YConstHelper.get();
        IConst_ = IConstHelper.get();
        SConst_ = SConstHelper.get();

        sgtLogDebug() << "YConst_.nnz() = " << YConst_.n_nonzero << std::endl;
        sgtLogDebug() << "IConst_.nnz() = " << IConst_.n_nonzero << std::endl;
        sgtLogDebug() << "SConst_.nnz() = " << SConst_.n_nonzero << std::endl;

        sgtLogDebug() << "PowerFlowModel : validate complete." << std::endl;
        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            print();
        }
    }

    void PowerFlowModel::print()
    {
        sgtLogDebug() << "PowerFlowModel::print()" << std::endl;
        sgtLogIndent();
        sgtLogDebug() << "Nodes:" << std::endl;
        {
            sgtLogIndent();
            for (const PfNode* nd : nodeVec_)
            {
                sgtLogDebug() << "Node:" << std::endl;
                {
                    sgtLogIndent();
                    sgtLogDebug() << "Id     : " << nd->bus_->id_ << std::endl;
                    sgtLogDebug() << "Type   : " << nd->bus_->type_ << std::endl;
                    sgtLogDebug() << "Phase  : " << nd->bus_->phases_[nd->phaseIdx_] << std::endl;
                    sgtLogDebug() << "V      : " << nd->V() << std::endl;
                    sgtLogDebug() << "SGen    : " << nd->SGen() << std::endl;
                }
            }
        }
        sgtLogDebug() << "Branches:" << std::endl;
        {
            sgtLogIndent();
            for (const std::unique_ptr<PfBranch>& branch : branchVec_)
            {
                sgtLogDebug() << "Branch:" << std::endl;
                {
                    sgtLogIndent();
                    sgtLogDebug() << "Buses : " << branch->ids_[0] << ", " << branch->ids_[1] << std::endl;
                    sgtLogDebug() << "Phases : " << branch->phases_[0] << ", " << branch->phases_[1] << std::endl;
                    sgtLogDebug() << "YConst :" << std::endl;
                    {
                        sgtLogIndent();
                        for (uword i = 0; i < branch->YConst_.n_rows; ++i)
                        {
                            sgtLogDebug() << std::setprecision(14) << std::setw(18) 
                                << branch->YConst_.row(i) << std::endl;
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

    Col<Complex> PowerFlowModel::SGen() const
    {
        auto it = nodeVec_.begin();
        return Col<Complex>(nodeVec_.size()).imbue([&](){return (**(it++)).SGen();});
    }

    void PowerFlowModel::setSGen(const Col<Complex>& SGen) const
    {
        for (uword i = 0; i < SGen.size(); ++i)
        {
            nodeVec_[i]->setSGen(SGen[i]);
        }
    }
}
