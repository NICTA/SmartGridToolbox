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

#include "SimMatpowerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/MatpowerParserPlugin.h>

namespace Sgt
{
    void SimMatpowerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        MatpowerParserPlugin mpParser;
        assertFieldPresent(nd, "network_id");
        string netwId = parser.expand<std::string>(nd["network_id"]);
        auto simNetw = sim.simComponent<SimNetwork>(netwId);

        Network tempNetw("temp", 100.0);
        mpParser.parse(nd, tempNetw, parser);

        // Now recreate the SimNetwork from the Network.
        for (auto& bus : tempNetw.busses())
        {
            auto simBus = sim.newSimComponent<SimBus>(*bus);
            simNetw->addBus(simBus);

            for (auto& gen : bus->gens())
            {
                auto gGen = dynamic_cast<const GenericGen&>(*gen);
                auto simGen = sim.newSimComponent<SimGenericGen>(gGen);
                simNetw->addGen(simGen, bus->id());
            }

            for (auto& zip : bus->zips())
            {
                auto gZip = dynamic_cast<const GenericZip&>(*zip);
                auto simZip = sim.newSimComponent<SimGenericZip>(gZip);
                simNetw->addZip(simZip, bus->id());
            }
        }

        for (auto& branch : tempNetw.branches())
        {
            auto cBranch = dynamic_cast<const CommonBranch&>(*branch);
            auto simCBranch = sim.newSimComponent<SimCommonBranch>(cBranch);
            simNetw->addBranch(simCBranch, branch->bus0()->id(), branch->bus1()->id());
        }
    }
}
