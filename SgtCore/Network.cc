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

#include "PowerFlowNrPolSolver.h"
#include "Zip.h"

#include <numeric>

namespace Sgt
{
    Network::Network(double PBase) : PBase_(PBase), solver_(new PowerFlowNrPolSolver)
    {
        // Empty.
    }

    void Network::addBus(std::shared_ptr<Bus> bus)
    {
        busMap_[bus->id()] = bus;
        busVec_.push_back(bus.get());
    }

    void Network::addBranch(std::shared_ptr<BranchAbc> branch, const std::string& bus0Id, const std::string& bus1Id)
    {
        Bus* bus0 = bus(bus0Id);
        Bus* bus1 = bus(bus1Id);
        sgtAssert(bus0 != nullptr, "Bus " << bus0Id << " was not found in the network.");
        sgtAssert(bus1 != nullptr, "Bus " << bus1Id << " was not found in the network.");
        branch->setBus0(*bus0);
        branch->setBus1(*bus1);
        branchMap_[branch->id()] = branch;
        branchVec_.push_back(branch.get());
    }

    void Network::addGen(std::shared_ptr<GenAbc> gen, const std::string& busId)
    {
        genMap_[gen->id()] = gen;
        genVec_.push_back(gen.get());
        Bus* bus = this->bus(busId);
        sgtAssert(bus != nullptr, "Bus " << busId << " was not found in the network.");
        bus->addGen(*gen);
    }

    void Network::addZip(std::shared_ptr<ZipAbc> zip, const std::string& busId)
    {
        zipMap_[zip->id()] = zip;
        zipVec_.push_back(zip.get());
        Bus* bus = this->bus(busId);
        sgtAssert(bus != nullptr, "Bus " << busId << " was not found in the network.");
        bus->addZip(*zip);
    }

    bool Network::solvePowerFlow()
    {
        sgtLogDebug() << "Network : solving power flow." << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << *this;

        if (useFlatStart_)
        {
            for (Bus* bus : busses())
            {
                bus->setV(bus->VNom());
            }
        }
        isValidSolution_ = solver_->solve(*this);
        if (!isValidSolution_)
        {
            sgtLogWarning() << "Couldn't solve power flow model" << std::endl;
        }

        return isValidSolution_;
    }
            
    double Network::genCostPerUnitTime()
    {
        return std::accumulate(genVec_.begin(), genVec_.end(), 0.0, 
                [](double d, GenAbc* g)->double{return d + g->cost();});
    }

    void Network::print(std::ostream& os) const
    {
        os << "Network:" << std::endl;
        StreamIndent _(os);
        os << "P_base: " << PBase_ << std::endl;
        for (const Bus* bus : busVec_)
        {
            {
                os << "Bus: " << std::endl;
                StreamIndent _1(os);
                os << *bus << std::endl;
            }
            {
                os << "Zips: " << std::endl;
                StreamIndent _1(os);
                for (const ZipAbc* zip : bus->zips())
                {
                    os << *zip << std::endl;
                }
            }
            {
                os << "Gens: " << std::endl;
                StreamIndent _(os);
                for (const GenAbc* gen : bus->gens())
                {
                    os << *gen << std::endl;
                }
            }
        }
        for (const BranchAbc* branch : branchVec_)
        {
            os << "Branch: " << std::endl;
            StreamIndent _1(os);
            os << *branch << std::endl;
            StreamIndent _2(os);
            os << "Bus 0 = " << branch->bus0()->id() << std::endl;
            os << "Bus 1 = " << branch->bus1()->id() << std::endl;
        }
    }

    std::unique_ptr<PowerFlowModel> buildModel(const Network& netw)
    {
        std::unique_ptr<PowerFlowModel> mod(new PowerFlowModel(netw.busses().size(), netw.branches().size()));
        for (Bus* bus : netw.busses())
        {
            if (bus->isInService())
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
                        bus->SGenRequested() + bus->SConst(), bus->JGen());

                bus->setType(busTypeSv);
                bus->setpointChanged().setIsEnabled(isEnabledSv);
            }
        }
        for (const BranchAbc* branch : netw.branches())
        {
            if (branch->isInService() && branch->bus0()->isInService() && branch->bus1()->isInService())
            {
                // TODO: ignore like this, or add the branch with zero admittance?
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
            Bus* bus = netw.bus(modBus.id_);

            int nInService = bus->nInServiceGens();

            arma::Col<Complex> SGen = nInService > 0
                                      ? (modBus.Scg_ - bus->SConst()) / nInService
                                      : arma::Col<Complex>(bus->phases().size(), arma::fill::zeros);
            // Note: we've already taken YConst and IConst explicitly into account, so this is correct.
            // KLUDGE: We're using a vector above, rather than "auto" (which gives some kind of expression type).
            // This is less efficient, but the latter gives errors in valgrind.
            // Also: regarding the nInService check, recall that if nInService = 0, the bus is treated as PQ for
            // the purpose of the solver.

            bus->setV(modBus.V_);
            bus->setSGenUnserved(arma::Col<Complex>(bus->phases().size(), arma::fill::zeros)); // TODO: allow unserved.
            bus->setSZipUnserved(arma::Col<Complex>(bus->phases().size(), arma::fill::zeros)); // TODO: allow unserved.
            switch (bus->type())
            {
                case BusType::SL:
                    for (GenAbc* gen : bus->gens())
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
                    for (GenAbc* gen : bus->gens())
                    {
                        if (gen->isInService())
                        {
                            // Keep P for gens, distribute Q amongst all gens.
                            arma::Col<Complex> SNew(gen->S().size());
                            for (arma::uword i = 0; i < SNew.size(); ++i)
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
