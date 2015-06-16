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

#include "SimNetworkParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include "../SgtCore/PowerFlowSolverParserPlugin.h"

namespace Sgt
{
    void SimNetworkParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "P_base");

        string id = parser.expand<std::string>(nd["id"]);
        double PBase = parser.expand<double>(nd["P_base"]);

        auto ndFreq = nd["freq_Hz"];

        auto ndSolver = nd["solver"];

        std::unique_ptr<Network> nw = std::unique_ptr<Network>(new Network(PBase));

        if (ndFreq)
        {
            nw->setNomFreq(parser.expand<double>(ndFreq));
            nw->setFreq(nw->nomFreq());
        }

        if (ndSolver)
        {
            PowerFlowSolverParserPlugin pfSolverParser;
            pfSolverParser.parse(ndSolver, *nw, parser);
        }

        sim.newSimComponent<SimNetwork>(id, std::move(nw));
    }
}
