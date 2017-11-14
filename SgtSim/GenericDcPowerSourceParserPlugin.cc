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

#include "GenericDcPowerSourceParserPlugin.h"

#include "DcPowerSource.h"
#include "Inverter.h"
#include "Simulation.h"

namespace Sgt
{
    void GenericDcPowerSourceParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "inverter_id");
        assertFieldPresent(nd, "dc_power");

        std::string id = parser.expand<std::string>(nd["id"]);
        auto comp = sim.newSimComponent<GenericDcPowerSource>(id);
        comp->setRequestedPDc(parser.expand<double>(nd["dc_power"]));

        const std::string inverterStr = parser.expand<std::string>(nd["inverter_id"]);
        auto inverterComp = sim.simComponent<InverterAbc>(inverterStr);
        sgtAssert(inverterComp != nullptr,
                "For component " << id << ", inverter " << inverterStr << " was not found in the simulation.");
        inverterComp->addDcPowerSource(comp);
    }
}
