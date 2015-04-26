#include <SgtCore/Gen.h>
#include <SgtCore/Network.h>
#include <SgtCore/Parser.h>
#include <SgtCore/PowerFlowPtPpSolver.h>
#include <SgtSim/Inverter.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/SimNetwork.h>
#include <SgtSim/SimParser.h>

#include <PowerTools++/Constraint.h>
#include <PowerTools++/Net.h>
#include <PowerTools++/PowerModel.h>

using namespace Sgt;

class PvInverter : public Inverter
{
    public:
        PvInverter(const std::string& id, const std::string& busId, Network& netw) :
            Inverter(id, Phase::BAL),
            gen_(new GenericGen(id + "_gen", Phase::BAL)),
            busId_(busId)
        {
            netw.addGen(gen_, busId);
            gen_->setPMax(0.0);
            gen_->setPMin(0.0);
            gen_->setQMax(Sgt::infinity);
            gen_->setQMin(Sgt::negInfinity);
        }

        virtual arma::Col<Sgt::Complex> SConst() const override
        {
            // All reactive power to be handled by the gen.
            return arma::Col<Sgt::Complex>(phases().size(), arma::fill::none).fill(P());
        }

        std::shared_ptr<const GenAbc> gen() const
        {
            return gen_;
        }

        void setMaxSMag(double maxSMag)
        {
            Inverter::setMaxSMag(maxSMag);
            gen_->setQMax(maxSMag);
        }

    public:
        std::shared_ptr<GenAbc> gen_;
        std::string busId_;
};

class PvInverterParserPlugin : public SimParserPlugin
{
    public:
        virtual const char* key() override
        {
            return "pv_inverter";
        }

    public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override
        {
            assertFieldPresent(nd, "id");
            assertFieldPresent(nd, "network_id");
            assertFieldPresent(nd, "bus_id");

            string id = parser.expand<std::string>(nd["id"]);
            const std::string busId = parser.expand<std::string>(nd["bus_id"]);

            auto& netw = *sim.simComponent<SimNetwork>("network")->network();
            auto inverter = sim.newSimComponent<PvInverter>(id, busId, netw);

            if (nd["efficiency"])
            {
                inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
            }

            if (nd["max_S_mag"])
            {
                inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
            }

            if (nd["min_power_factor"])
            {
                inverter->setMinPowerFactor(parser.expand<double>(nd["min_power_factor"]));
            }

            const std::string networkId = parser.expand<std::string>(nd["network_id"]);

            auto network = sim.simComponent<SimNetwork>(networkId);
            network->addZip(inverter, busId);
        }
};

class PvDemoSolver : public PowerFlowPtPpSolver
{
    protected:
        virtual std::unique_ptr<PowerModel> makeModel(const Network& sgtNetw, Net& ptNetw) override
        {
            auto mod = PowerFlowPtPpSolver::makeModel(sgtNetw, ptNetw);
            for (auto zip : sgtNetw.zips())
            {
                auto inverter = std::dynamic_pointer_cast<PvInverter>(zip);
                if (inverter != nullptr)
                {
                    // Add an extra constraint for max apparent power.
                    auto bus = ptNetw.nodeID.at(inverter->busId_);
                    auto genId = inverter->gen_->id();
                    auto gen = std::find_if(bus->_gen.begin(), bus->_gen.end(), 
                            [genId](Gen* gen){return gen->_name == genId;});
                    assert(gen != bus->_gen.end());
                    auto c = new Constraint("PVD_SPECIAL");
                    *c += ((**gen).qg)^2;
                    *c <= inverter->maxSMag() * inverter->maxSMag() - pow(inverter->SConst()(0).real(), 2);
                    std::cout << "A: " << inverter->maxSMag() << std::endl;
                    std::cout << "B: " << inverter->SConst() << std::endl;
                    c->print(); std::cout << std::endl;
                    mod->_model->addConstraint(c);
                }
            }
            // mod->_model->print();
            return mod;
        }
};

int main(int argc, const char ** argv)
{
    using namespace Sgt;

    const char * configName = argv[1];
    const char * outputName = argv[2];
    double vlb = atof(argv[3]);
    double vub = atof(argv[4]);

    std::cout << "Configuration filename = " << configName << std::endl;
    std::cout << "Output filename        = " << outputName << std::endl;
    std::cout << "Voltage lower bound    = " << vlb << std::endl;
    std::cout << "Voltage upper bound    = " << vub << std::endl;

    std::ofstream outFile(outputName);

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<PvInverterParserPlugin>();
    p.parse(configName, sim);
    sim.initialize();

    SimNetwork& simNetwork = *sim.simComponent<SimNetwork>("network");
    Network& network = *simNetwork.network();
    for (auto bus : network.busses())
    {
        bus->setVMagMin(vlb * bus->VBase());
        bus->setVMagMax(vub * bus->VBase());
    }
    auto& bus = *sim.simComponent<Sgt::Bus>("bus_6");
    auto& inv = *sim.simComponent<PvInverter>("pv_inverter_6");
    simNetwork.network()->setSolver(std::unique_ptr<Sgt::PowerFlowSolverInterface>(new PvDemoSolver));
    
    while (!sim.isFinished())
    {
        std::cout 
            << "OUTPUT"
            << " " << dSeconds(sim.currentTime() - sim.startTime()) / 60
            << " " << inv.SConst()(0).real()
            << " " << inv.gen_->S()(0).imag()
            << " " << std::abs(bus.V()(0))/bus.VBase()
            << std::endl;
        LogIndent _;
        sim.doTimestep();
    }
}
