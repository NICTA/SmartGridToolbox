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

#include "YyTransformerParserPlugin.h"

#include "YyTransformer.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void YyTransformerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto trans = parseYyTransformer(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(trans), bus0Id, bus1Id);
    }

    std::unique_ptr<YyTransformer> YyTransformerParserPlugin::parseYyTransformer(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "complex_turns_ratio_01");
        assertFieldPresent(nd, "leakage_impedance");
        assertFieldPresent(nd, "phases_0");
        assertFieldPresent(nd, "phases_1");

        const std::string id = parser.expand<std::string>(nd["id"]);
        Complex a = parser.expand<Complex>(nd["complex_turns_ratio_01"]);
        Complex ZL = parser.expand<Complex>(nd["leakage_impedance"]);
        auto ndYm = nd["magnetizing_admittance"];
        Complex YM = ndYm ? parser.expand<Complex>(ndYm) : Complex(0.0, 0.0);
        const Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
        const Phases phases1 = parser.expand<Phases>(nd["phases_1"]);

        std::unique_ptr<YyTransformer> trans(new YyTransformer(id, a, ZL, YM, phases0, phases1));

        return trans;
    }
}
