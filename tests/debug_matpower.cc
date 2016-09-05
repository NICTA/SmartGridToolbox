#include "../SgtCore/SgtCore.h"

#include <fstream>

using namespace Sgt;
using namespace arma;
using namespace std;

int main(int argc, const char** argv)
{
    std::string caseName = argv[1];
    std::string solverType = argv[2];

    std::string yamlStr = std::string("--- [{matpower : {input_file : matpower_test_cases/") 
        + caseName + ".m, default_kV_base : 11}, power_flow_solver : " + solverType + "}]";

    Network nw(100.0);
    nw.setUseFlatStart(true);
    YAML::Node n = YAML::Load(yamlStr);
    Parser<Network> p;
    p.parse(n, nw);

    Stopwatch sw;
    sw.start();
    nw.solvePowerFlow();
    sw.stop();

    ofstream outSgt("debug_out_sgt.txt");
    ofstream outMp("debug_out_mp.txt");

    ifstream compareName(std::string("mp_compare/") + caseName + ".compare");
    for (auto bus : nw.buses())
    {
        double Vr, Vi, P, Q;
        compareName >> Vr >> Vi >> P >> Q;
        assert(!compareName.eof());
        Complex V = {Vr, Vi};
        Complex S = {P, Q};
        outSgt 
            << setw(8) << bus->id() << " " << setw(4) << bus->type() << ",    " 
            << "VM " << setw(8) << fixed << setprecision(5) << abs(bus->V()(0) / bus->VBase()) << ",    "
            << "VA " << setw(8) << fixed << setprecision(5) << arg(bus->V()(0)) * 180 / pi << ",    "
            << "PG " << setw(8) << fixed << setprecision(5) << real(bus->SGen()(0)) << ",    "
            << "QG " << setw(8) << fixed << setprecision(5) << imag(bus->SGen()(0)) << std::endl;
        outMp
            << setw(8) << bus->id() << " " << setw(4) << bus->type() << ",    "
            << "VM " << setw(8) << fixed << setprecision(5) << abs(V) << ",    "
            << "VA " << setw(8) << fixed << setprecision(5) << arg(V) * 180 / pi << ",    "
            << "PG " << setw(8) << fixed << setprecision(5) << real(S) << ",    "
            << "QG " << setw(8) << fixed << setprecision(5) << imag(S) << std::endl;
    }

    system("gvim -d debug_out_sgt.txt debug_out_mp.txt");
}
