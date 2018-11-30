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

#include "VvTransformerParserPlugin.h"

#include "VvTransformer.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void VvTransformerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto trans = parseVvTransformer(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(trans), bus0Id, bus1Id);
    }

    std::unique_ptr<VvTransformer> VvTransformerParserPlugin::parseVvTransformer(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "nom_ratio_w1");
        assertFieldPresent(nd, "nom_ratio_w2");
        assertFieldPresent(nd, "leakage_impedance_w1");
        assertFieldPresent(nd, "leakage_impedance_w2");
        assertFieldPresent(nd, "phases_0");
        assertFieldPresent(nd, "phases_1");

        const std::string id = parser.expand<std::string>(nd["id"]);
        Complex nomRatioW1 = parser.expand<Complex>(nd["nom_ratio_w1"]);
        Complex nomRatioW2 = parser.expand<Complex>(nd["nom_ratio_w2"]);

        auto ndOffNomRatioW1 = nd["off_nom_ratio_w1"];
        Complex offNomRatioW1 = ndOffNomRatioW1 ? parser.expand<Complex>(nd["off_nom_ratio_w1"]) : 1.0;
        
        auto ndOffNomRatioW2 = nd["off_nom_ratio_w2"];
        Complex offNomRatioW2 = ndOffNomRatioW2 ? parser.expand<Complex>(nd["off_nom_ratio_w2"]) : 1.0;

        Complex ZLW1 = parser.expand<Complex>(nd["leakage_impedance_w1"]);
        Complex ZLW2 = parser.expand<Complex>(nd["leakage_impedance_w2"]);

        const Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
        const Phases phases1 = parser.expand<Phases>(nd["phases_1"]);
        
        std::unique_ptr<VvTransformer> trans(
                new VvTransformer(id, nomRatioW1, nomRatioW2, offNomRatioW1, offNomRatioW2, ZLW1, ZLW2,
                    phases0, phases1));

        return trans;
    }
}
