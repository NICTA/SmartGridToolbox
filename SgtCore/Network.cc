#include "Network.h"

#include "PowerFlowNrSolver.h"
#include "Zip.h"

#include <numeric>

namespace Sgt
{
    Network::Network(const std::string& id, double PBase) :
        Component(id),
        PBase_(PBase),
        solver_(new PowerFlowNrSolver)
    {
        // Empty.
    }

    void Network::addBus(BusPtr bus)
    {
        busMap_[bus->id()] = bus;
        busVec_.push_back(bus);
    }

    void Network::addBranch(BranchPtr branch, const std::string& bus0Id, const std::string& bus1Id)
    {
        auto bus0 = bus(bus0Id);
        auto bus1 = bus(bus1Id);
        if (bus0 == nullptr)
        {
            Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus0Id << " was not found in the network." << std::endl;
        }
        if (bus1 == nullptr)
        {
            Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << bus1Id << " was not found in the network." << std::endl;
        }
        branch->setBus0(bus0);
        branch->setBus1(bus1);
        branchMap_[branch->id()] = branch;
        branchVec_.push_back(branch);
    }

    void Network::addGen(GenPtr gen, const std::string& busId)
    {
        genMap_[gen->id()] = gen;
        genVec_.push_back(gen);
        auto bus = this->bus(busId);
        if (bus != nullptr)
        {
            bus->addGen(gen);
        }
        else
        {
            Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << busId << " was not found in the network." << std::endl;
        }
    }

    void Network::addZip(ZipPtr zip, const std::string& busId)
    {
        zipMap_[zip->id()] = zip;
        zipVec_.push_back(zip);
        auto bus = this->bus(busId);
        if (bus != nullptr)
        {
            bus->addZip(zip);
        }
        else
        {
            Log().fatal() << __PRETTY_FUNCTION__ << " : Bus " << busId << " was not found in the network." << std::endl;
        }
    }

    bool Network::solvePowerFlow()
    {
        SGT_DEBUG(Log().debug() << "Network : solving power flow." << std::endl);
        SGT_DEBUG(Log().debug() << *this);

        isValidSolution_ = solver_->solve(this);
        if (!isValidSolution_)
        {
            Log().warning() << "Couldn't solve power flow model" << std::endl;
        }

        return isValidSolution_;
    }

    void Network::print(std::ostream& os) const
    {
        Component::print(os);
        StreamIndent _(os);
        os << "P_base: " << PBase_ << std::endl;
        for (auto bus : busVec_)
        {
            {
                os << "Bus: " << std::endl;
                StreamIndent _(os);
                os << *bus << std::endl;
            }
            {
                os << "Zips: " << std::endl;
                StreamIndent _(os);
                for (auto zip : bus->zips())
                {
                    os << *zip << std::endl;
                }
            }
            {
                os << "Gens: " << std::endl;
                StreamIndent _(os);
                for (auto gen : bus->gens())
                {
                    os << *gen << std::endl;
                }
            }
        }
        for (auto branch : branchVec_)
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
        std::unique_ptr<PowerFlowModel> mod(new PowerFlowModel);
        for (const auto bus : netw.busses())
        {
            bool isEnabledSv = bus->setpointChanged().isEnabled();
            bus->setpointChanged().setIsEnabled(false);
            BusType busTypeSv = bus->type();
            if (bus->type() != BusType::PQ)
            {
                if (bus->nInServiceGens() == 0)
                {
                    Log().warning() << "Bus " << bus->id() << " has type " << bus->type()
                                    << ", but does not have any in service generators. Temporarily setting type to PQ." << std::endl;
                    bus->setType(BusType::PQ);
                }
            }

            bus->applyVSetpoints();
            mod->addBus(bus->id(), bus->type(), bus->phases(), bus->YConst(), bus->IConst(), bus->SConst(), bus->JGen(),
                        bus->V(), bus->SGenRequested() + bus->SConst());

            bus->setType(busTypeSv);
            bus->setpointChanged().setIsEnabled(isEnabledSv);
        }
        for (const auto branch : netw.branches())
        {
            if (branch->isInService())
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
        for (const auto& busPair: mod.busses())
        {
            auto& modBus = *busPair.second;
            const auto bus = netw.bus(modBus.id_);

            int nInService = bus->nInServiceGens();

            arma::Col<Complex> SGen = nInService > 0
                                      ? (modBus.S_ - bus->SConst()) / nInService
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
                    for (const auto& gen : bus->gens())
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
                    for (const auto& gen : bus->gens())
                    {
                        if (gen->isInService())
                        {
                            // Keep P for gens, distribute Q amongst all gens.
                            arma::Col<Complex> SNew(gen->S().size());
                            for (int i = 0; i < SNew.size(); ++i)
                            {
                                SNew[i] = Complex(gen->S()[i].real(), SGen[i].imag());
                            }
                            gen->setInServiceS(SNew);
                        }
                    }
                    break;
                default:
                    Log().fatal() << "Bad bus type." << std::endl;
            }
        }
    }
}
