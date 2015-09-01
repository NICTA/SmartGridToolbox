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
#include "PowerFlowNrSolver.h"
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
            sgtLogMessage() << "Using Newton-Raphson solver." << std::endl;
            netw.setSolver(std::unique_ptr<PowerFlowNrSolver>(new PowerFlowNrSolver));
        }
        else if (key == "opf_pt_pp")
        {
#ifdef ENABLE_POWER_TOOLS_PP
            sgtLogMessage() << "Using OPF (PowerTools++) solver." << std::endl;
            netw.setSolver(std::unique_ptr<PowerFlowPtPpSolver>(new PowerFlowPtPpSolver));
#else // ENABLE_POWER_TOOLS_PP
            sgtError("OPF solver is not available, since SmartGridToolbox was not compiled with "
                     "--enable-power-tools++.");
#endif // ENABLE_POWER_TOOLS_PP
        }
    }
}
