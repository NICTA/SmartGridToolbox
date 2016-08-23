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

#include "Network.h"

#include "PowerFlowNrRectSolver.h"
#include "Zip.h"

#include <numeric>

using namespace arma;

namespace Sgt
{
    namespace
    {
        void islandDfs(ComponentPtr<Bus> bus, Island& island)
        {
            if (bus->islandIdx() != -1 || !bus->isInService()) return;

            bus->setIslandIdx(island.idx);
            island.buses.push_back(bus);
            for (auto branch : bus->branches0())
            {
                if (branch->isInService()) islandDfs(branch->bus1(), island);
            }
            for (auto branch : bus->branches1())
            {
                if (branch->isInService()) islandDfs(branch->bus0(), island);
            }
        }
    }

    Network::Network(double PBase) : PBase_(PBase), solver_(new PowerFlowNrRectSolver)
    {
        // Empty.
    }

    void Network::addBranch(std::shared_ptr<BranchAbc> branch, const std::string& bus0Id, const std::string& bus1Id)
    {
        auto bus0 = buses_[bus0Id];
        sgtAssert(bus0 != nullptr, "Bus " << bus0Id << " was not found in the network.");
        auto bus1 = buses_[bus0Id];
        sgtAssert(bus1 != nullptr, "Bus " << bus1Id << " was not found in the network.");

        branches_.insert(branch);

        branch->bus0_ = bus0;
        branch->bus1_ = bus1;

        bus0->branches0_.insert(branch);
        bus1->branches1_.insert(branch);
    }

    void Network::addGen(std::shared_ptr<GenAbc> gen, const std::string& busId)
    {
        auto bus = buses_[busId];
        sgtAssert(bus != nullptr, "Bus " << busId << " was not found in the network.");

        gens_.insert(gen);
        gen->bus_ = bus;
        bus->gens_.insert(gen);
    }

    void Network::addZip(std::shared_ptr<ZipAbc> zip, const std::string& busId)
    {
        auto bus = buses_[busId];
        sgtAssert(bus != nullptr, "Bus " << busId << " was not found in the network.");

        zips_.insert(zip);
        zip->bus_ = bus;
        bus->zips_.insert(zip);
    }

    void Network::applyFlatStart()
    {
        for (auto bus : buses())
        {
            bus->V_ = bus->VNom();

            for (auto gen : bus->gens())
            {
                switch (bus->type())
                {
                    case BusType::SL:
                        {
                            // Set SGen to 0.
                            gen->setInServiceS(Col<Complex>(gen->phases().size(), fill::zeros));
                            break;
                        }
                    case BusType::PV:
                        {
                            // Set QGen to 0.
                            auto S = gen->inServiceS();
                            S.set_imag(zeros(gen->phases().size()));
                            gen->setInServiceS(S);
                            break;
                        }
                    default: 
                        {
                            break;
                        }
                }
            }
        }
    }

    bool Network::solvePowerFlow()
    {
        sgtLogDebug() << "Network : solving power flow." << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << *this << std::endl;

        // Preprocess.
        
        // Flat start.
        if (useFlatStart_)
        {
            applyFlatStart();
        }

        // Islands.
        findIslands();
        handleUnsuppliedIslands();

        // Solve and update network.
        isValidSolution_ = solver_->solve(*this);
        if (!isValidSolution_)
        {
            sgtLogWarning() << "Couldn't solve power flow model" << std::endl;
        }
        
        // Postprocess.
        // TODO.

        return isValidSolution_;
    }
            
    double Network::genCostPerUnitTime()
    {
        return std::accumulate(gens_.begin(), gens_.end(), 0.0, 
                [](double d, GenAbc* g)->double{return d + g->cost();});
    }
            
    void Network::findIslands()
    {
        std::map<std::string, ComponentPtr<Bus>> remaining; 
        for (auto bus : buses_) remaining[bus->id()] = bus;

        // Step 1: Initialize.
        islands_.clear();
        for (auto bus : buses()) bus->islandIdx_ = -1;
        int curIdx = 0;

        // Step 2: DFS from all in-service buses with a working generator.
        for (auto gen : gens())
        {
            if (!gen->isInService()) continue;
            auto bus = gen->bus();
            if (!bus->isInService()) continue;

            Island island{curIdx, true, {}};
            islandDfs(bus, island);
            if (island.buses.size() > 0)
            {
                for (auto b : island.buses) remaining.erase(b->id());
                islands_.push_back(std::move(island));
                ++curIdx;
            }
        }
        
        // Step 3: Do all unsupplied and out-of-service buses.
        while (!remaining.empty())
        {
            auto bus = remaining.begin()->second;
            if (!bus->isInService())
            { 
                // Out of service buses get their own island.
                islands_.push_back({curIdx++, false, {bus}});
                remaining.erase(bus->id());
            }
            else
            {
                // Should be unsupplied. Note that the island will contain at least bus.
                islands_.push_back({curIdx++, false, {}});
                islandDfs(bus, islands_.back());
                for (auto b : islands_.back().buses) remaining.erase(b->id());
            }
        }
    }
            
    void Network::handleUnsuppliedIslands()
    {
        for (auto island : islands_)
        {
            if (island.isSupplied) continue;

            for (auto bus : island.buses)
            {
                bus->setV(arma::Col<Complex>(bus->V().size(), arma::fill::zeros));
                bus->setSGenUnserved(bus->SGenRequested());
                bus->setSZipUnserved(bus->SZipRequested());
            }
        }
    }
    
    json Network::toJson() const
    {
        json j;
        j["network"] = {
            {"p_base", PBase()},
            {"buses", buses()},
            {"branches", branches()},
            {"gens", gens()},
            {"zips", zips()}
        };
        return j;
    }

    std::unique_ptr<PowerFlowModel> buildModel(const Network& netw,
            const std::function<bool (const Bus&)> selBus)
    {
        std::unique_ptr<PowerFlowModel> mod(new PowerFlowModel(netw.buses().size(), netw.branches().size()));
        for (auto bus : netw.buses())
        {
            if (selBus(*bus) && bus->isInService())
            {
                bool isEnabledSv = bus->setpointChanged().isEnabled();
                bus->setpointChanged().setIsEnabled(false);
                BusType busTypeSv = bus->type();
                if (bus->type() != BusType::PQ)
                {
                    if (bus->nInServiceGens() == 0)
                    {
                        sgtLogWarning(LogLevel::VERBOSE) << "Bus " << bus->id() << " has type " << bus->type()
                            << ", but does not have any in service generators. Temporarily setting type to PQ."
                            << std::endl;
                        bus->setType(BusType::PQ);
                    }
                }

                bus->applyVSetpoints();
                mod->addBus(bus->id(), bus->type(), bus->phases(), bus->V(), bus->YConst(), bus->IConst(),
                        bus->SConst(), bus->SGenRequested(), bus->JGen());

                bus->setType(busTypeSv);
                bus->setpointChanged().setIsEnabled(isEnabledSv);
            }
        }
        for (auto branch : netw.branches())
        {
            if (branch->isInService() && 
                    selBus(*branch->bus0()) && branch->bus0()->isInService() &&
                    selBus(*branch->bus1()) && branch->bus1()->isInService())
            {
                mod->addBranch(branch->bus0()->id(), branch->bus1()->id(),
                        branch->phases0(), branch->phases1(), branch->Y());
            }
        }
        mod->validate();
        return mod;
    }

    void applyModel(const PowerFlowModel& mod, Network& netw)
    {
        for (const auto& busPair: mod.busMap())
        {
            auto& modBus = *busPair.second;
            auto bus = netw.buses()[modBus.id_];

            int nInService = bus->nInServiceGens();

            Col<Complex> SGen = nInService > 0
                                      ? (modBus.SGen_) / nInService
                                      : Col<Complex>(bus->phases().size(), fill::zeros);

            bus->setV(modBus.V_);
            bus->setSGenUnserved(Col<Complex>(bus->phases().size(), fill::zeros));
            bus->setSZipUnserved(Mat<Complex>(bus->phases().size(), bus->phases().size(), fill::zeros)); 
            // TODO: allow unserved.
            switch (bus->type())
            {
                case BusType::SL:
                    for (auto gen : bus->gens())
                    {
                        if (gen->isInService())
                        {
                            gen->setInServiceS(SGen);
                        }
                    }
                    break;
                case BusType::PQ:
                    break;
                case BusType::PV:
                    for (auto gen : bus->gens())
                    {
                        if (gen->isInService())
                        {
                            // Keep P for gens, distribute Q amongst all gens.
                            Col<Complex> SNew(gen->S().size());
                            for (uword i = 0; i < SNew.size(); ++i)
                            {
                                SNew(i) = Complex(gen->S()(i).real(), SGen(i).imag());
                            }
                            gen->setInServiceS(SNew);
                        }
                    }
                    break;
                default:
                    sgtError("Bad bus type.");
            }
        }
    }
}
