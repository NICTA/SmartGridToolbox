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

#include "GenericBranchParserPlugin.h"

#include "Branch.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void GenericBranchParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto gb = parseGenericBranch(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(gb), bus0Id, bus1Id);
    }

    std::unique_ptr<GenericBranch> GenericBranchParserPlugin::parseGenericBranch(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases_0");
        assertFieldPresent(nd, "phases_1");
        assertFieldPresent(nd, "Y");

        const std::string id = parser.expand<std::string>(nd["id"]);
        const Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
        const Phases phases1 = parser.expand<Phases>(nd["phases_1"]);

        const YAML::Node ndY = nd["Y"];

        const YAML::Node ndYMatrix = ndY["matrix"];
        const YAML::Node ndYApproxPhaseImpedance = ndY["approximate_phase_impedance"];
        arma::Mat<Complex> Y(phases0.size() + phases1.size(), phases0.size() + phases1.size(), arma::fill::zeros);
        if (ndYMatrix)
        {
            Y = parser.expand<arma::Mat<Complex>>(ndYMatrix);
        }
        else if (ndYApproxPhaseImpedance)
        {
            Complex ZPlus = parser.expand<Complex>(ndYApproxPhaseImpedance["Z+"]);
            Complex Z0 = parser.expand<Complex>(ndYApproxPhaseImpedance["Z0"]);
            Y = ZLine2YNode(approxPhaseImpedanceMatrix(ZPlus, Z0));
        }
        
        const YAML::Node ndImpedMult = ndY["impedance_multiplier"];
        if (ndImpedMult)
        {
            Y /= ndImpedMult.as<double>();
        }

        std::unique_ptr<GenericBranch> branch(new GenericBranch(id, phases0, phases1));

        branch->setInServiceY(Y);

        return branch;
    }
}
