#include "config.h"

#include "PowerFlowSolverParserPlugin.h"

#include "Network.h"
#include "PowerFlowNrSolver.h"
#ifdef ENABLE_POWER_TOOLS
#include "PowerFlowPtSolver.h"
#endif
#ifdef ENABLE_POWER_TOOLS_PP
#include "PowerFlowPtPpSolver.h"
#endif
#include "YamlSupport.h"

namespace Sgt
{
    void PowerFlowSolverParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto key = nd.as<std::string>();
        if (key == "nr")
        {
            Log().message() << "Using Newton-Raphson solver." << std::endl;
            netw.setSolver(std::unique_ptr<PowerFlowNrSolver>(new PowerFlowNrSolver));
        }
        else if (key == "opf_pt")
        {
#ifdef ENABLE_POWER_TOOLS
            Log().message() << "Using OPF (PowerTools) solver." << std::endl;
            netw.setSolver(std::unique_ptr<PowerFlowPtSolver>(new PowerFlowPtSolver));
#else // ENABLE_POWER_TOOLS 
            Log().fatal()
                    << "OPF solver is not available, since SmartGridToolbox was not compiled with --enable-power-tools."
                    << std::endl;
#endif // ENABLE_POWER_TOOLS
        }
        else if (key == "opf_pt_pp")
        {
#ifdef ENABLE_POWER_TOOLS_PP
            Log().message() << "Using OPF (PowerTools++) solver." << std::endl;
            netw.setSolver(std::unique_ptr<PowerFlowPtPpSolver>(new PowerFlowPtPpSolver));
#else // ENABLE_POWER_TOOLS_PP
            Log().fatal()
                    << "OPF solver is not available, since SmartGridToolbox was not compiled with --enable-power-tools++."
                    << std::endl;
#endif // ENABLE_POWER_TOOLS_PP
        }
    }
}
