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

namespace Sgt
{
    class PvInverter : public SimpleInverterAbc, public GenericGen
    {
        public:

            /// @name Static member functions:
            /// @{

        public:

            static const std::string& sComponentType()
            {
                static std::string result("pv_inverter");
                return result;
            }

            /// @}

            PvInverter(const std::string& id, std::string busId) :
                GenericGen(id, Phase::BAL),
                busId_(busId)
            {
                // Empty.
            }

            /// @name ComponentInterface virtual overridden functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            /// @}

            /// @name InverterAbc virtual overridden functions.
            /// @{

            virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source) override
            {
                InverterAbc::addDcPowerSource(source);
                source->dcPowerChanged().addAction([this]() {setPMax(availableP());}, "Update max power");
            }

            /// @}

            /// @name SimpleZipInverter specific member functions.
            /// @{

            double maxSMag() const
            {
                return maxSMag_;
            }

            void setMaxSMag(double maxSMag)
            {
                maxSMag_ = maxSMag;
            }

            /// @}

        public:

            double maxSMag_{1e9};
            double requestedQ_{0.0};
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

                auto inverter = sim.newSimComponent<PvInverter>(id, busId);

                if (nd["efficiency"])
                {
                    inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
                }

                if (nd["max_S_mag"])
                {
                    inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
                }

                const std::string networkId = parser.expand<std::string>(nd["network_id"]);
                auto network = sim.simComponent<SimNetwork>(networkId);
                network->addGen(inverter, busId);
            }
    };

    class PvDemoSolver : public PowerFlowPtPpSolver
    {
        protected:
            virtual std::unique_ptr<PowerModel> makeModel(const Network& sgtNetw, Net& ptNetw) override
            {
                auto mod = PowerFlowPtPpSolver::makeModel(sgtNetw, ptNetw);
                for (auto gen : sgtNetw.gens())
                {
                    auto inverter = std::dynamic_pointer_cast<PvInverter>(gen);
                    if (inverter != nullptr)
                    {
                        // Add an extra constraint for max apparent power.
                        auto bus = ptNetw.nodeID.at(inverter->busId_);
                        auto genId = inverter->id();
                        auto gen = std::find_if(bus->_gen.begin(), bus->_gen.end(), 
                                [genId](Gen* gen){return gen->_name == genId;});
                        assert(gen != bus->_gen.end());

                        {
                            auto c = new Constraint("PVD_SPECIAL_A");
                            *c += ((**gen).pg)^2;
                            *c += ((**gen).qg)^2;
                            double maxSMag = sgtNetw.S2Pu(inverter->maxSMag());
                            *c <= pow(maxSMag, 2);
                            c->print(); std::cout << std::endl;
                            mod->_model->addConstraint(c);
                        }

                        {
                            auto c = new Constraint("PVD_SPECIAL_B");
                            *c += ((**gen).pg)^2;
                            *c -= ((**gen).qg)^2;
                            *c >= 0;
                            c->print(); std::cout << std::endl;
                            mod->_model->addConstraint(c);
                        }
                    }
                }
                // mod->_model->print();
                return mod;
            }
    };
}

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
        std::cout << "TIME " << sim.currentTime() << std::endl;
        std::cout 
            << "OUTPUT"
            << " " << dSeconds(sim.currentTime() - sim.startTime()) / 60
            << " " << inv.S()(0).real()
            << " " << inv.S()(0).imag()
            << " " << std::abs(bus.V()(0))/bus.VBase()
            << std::endl;
        LogIndent _;
        sim.doTimestep();
    }
}
