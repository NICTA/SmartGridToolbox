// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include "SinglePhaseTransformerParserPlugin.h"

#include "SinglePhaseTransformer.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void SinglePhaseTransformerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto trans = parseSinglePhaseTransformer(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(trans), bus0Id, bus1Id);
    }

    std::unique_ptr<SinglePhaseTransformer> SinglePhaseTransformerParserPlugin::parseSinglePhaseTransformer(
        const YAML::Node& nd, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "nom_V_ratio");
        assertFieldPresent(nd, "leakage_impedance");
        assertFieldPresent(nd, "phase_0");
        assertFieldPresent(nd, "phase_1");

        const std::string id = parser.expand<std::string>(nd["id"]);
        Complex nomVRatio = parser.expand<Complex>(nd["nom_V_ratio"]);
        auto ndOffNomRatio = nd["off_nom_ratio"];
        Complex offNomRatio = ndOffNomRatio ? parser.expand<Complex>(nd["off_nom_ratio"]) : 1.0;
        Complex ZL = parser.expand<Complex>(nd["leakage_impedance"]);
        const Phase phase0 = parser.expand<Phase>(nd["phase_0"]);
        const Phase phase1 = parser.expand<Phase>(nd["phase_1"]);

        std::unique_ptr<SinglePhaseTransformer> trans(new SinglePhaseTransformer(id, nomVRatio, offNomRatio, ZL,
                    phase0, phase1));

        return trans;
    }
}
