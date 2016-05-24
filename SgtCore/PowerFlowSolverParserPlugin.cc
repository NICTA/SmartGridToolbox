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

#include "config.h"

#include "PowerFlowSolverParserPlugin.h"

#include "Network.h"
#include "PowerFlowNrPolSolver.h"
#include "PowerFlowNrRectSolver.h"
#include "PowerFlowFdSolver.h"
#ifdef ENABLE_POWER_TOOLS
#include "PowerFlowPtSolver.h"
#endif
#include "YamlSupport.h"

namespace Sgt
{
    void PowerFlowSolverParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto key = nd.as<std::string>();
        if (key == "nr_rect")
        {
            sgtLogMessage() << "Using Newton-Raphson (rectangular) solver." << std::endl;
            netw.setSolver(std::make_unique<PowerFlowNrRectSolver>());
        }
        if (key == "nr_pol")
        {
            sgtLogMessage() << "Using Newton-Raphson (polar) solver." << std::endl;
            netw.setSolver(std::make_unique<PowerFlowNrPolSolver>());
        }
        else if (key == "fd")
        {
            sgtLogMessage() << "Using fast-decoupled load flow solver." << std::endl;
            netw.setSolver(std::make_unique<PowerFlowFdSolver>());
        }
        else if (key == "opf_pt")
        {
#ifdef ENABLE_POWER_TOOLS
            sgtLogMessage() << "Using OPF (PowerTools) solver." << std::endl;
            netw.setSolver(std::make_unique<PowerFlowPtSolver>());
#else // ENABLE_POWER_TOOLS
            sgtError("OPF solver is not available, since SmartGridToolbox was not compiled with "
                     "--enable-power-tools++.");
#endif // ENABLE_POWER_TOOLS
        }
    }
}
