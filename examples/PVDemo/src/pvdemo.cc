#include <SgtCore/Gen.h>
#include <SgtCore/Network.h>
#include <SgtCore/PowerFlowPtPpSolver.h>
#include <SgtSim/Inverter.h>

class PVDemoInverter : public Sgt::Inverter
{
    PVDemoInverter(const std::string& id, std::shared_ptr<Sgt::GenAbc> gen) :
        Sgt::Inverter(id, Sgt::Phase::BAL),
        gen_(gen)
    {
        // Empty.
    }
    public:
        std::shared_ptr<const Sgt::GenAbc> gen() const
        {
            return gen_;
        }
    public:
        std::shared_ptr<Sgt::GenAbc> gen_;
};

class PVDemoSolver : public Sgt::PowerFlowPtPpSolver
{
    protected:
        virtual std::unique_ptr<PowerModel> getModel(const Sgt::Network& sgtNetw, Net& ptNetw) override
        {
            auto mod = Sgt::PowerFlowPtPpSolver::getModel(sgtNetw, ptNetw);
            for (auto zip : sgtNetw.zips())
            {
                auto inverter = std::dynamic_pointer_cast<PVDemoInverter>(zip);
                if (inverter != nullptr)
                {
                    // Add an extra constraint for max apparent power.
                    auto busId = inverter->bus()->id();
                    auto bus = ptNetw.busId(busId);
                    auto genId = inverter->gen()->id();
                    auto gen = 
                        std::find_if(bus->_gen.begin(), bus->_gen.end(), [](Gen* gen){return gen->id() == genId;});
                    assert(gen != nullptr);
                    auto c = new Constraint("PVD_S2");
                    *c += gen->pg^2 + gen->qg^2;
                    *c = inverter->maxSMagPerPhase();
                    mod->addConstraint(c);
                }
            }
        }
};

int main(int argc, const char ** argv)
{
   const char * configName = argv[1];
   const char * outputName = argv[2];
   double vlb = atof(argv[3]);
   double vub = atof(argv[4]);

   std::cout << "Configuration filename = " << configName << std::endl;
   std::cout << "Output filename        = " << outputName << std::endl;
   std::cout << "Voltage lower bound    = " << vlb << std::endl;
   std::cout << "Voltage upper bound    = " << vub << std::endl;

   std::ofstream outFile(outputName);
}
