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

#include "CommonBranchParserPlugin.h"

#include "CommonBranch.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void CommonBranchParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        auto cBranch = parseCommonBranch(nd, parser);

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(cBranch), bus0Id, bus1Id);
    }

    std::unique_ptr<CommonBranch> CommonBranchParserPlugin::parseCommonBranch(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "Y_series");

        std::string id = parser.expand<std::string>(nd["id"]);

        std::unique_ptr<CommonBranch> cBranch(new CommonBranch(id));

        cBranch->setYSeries(parser.expand<Complex>(nd["Y_series"]));
        
        auto ndShunt = nd["Y_shunt"];
        if (ndShunt)
        {
            cBranch->setYShunt(parser.expand<Complex>(ndShunt));
        }

        auto ndTap = nd["complex_tap_ratio"];
        if (ndTap)
        {
            cBranch->setTapRatio(parser.expand<Complex>(ndTap));
        }

        auto ndRateA =  nd["rate_A"];
        auto ndRateB =  nd["rate_B"];
        auto ndRateC =  nd["rate_C"];

        if (ndRateA)
        {
            cBranch->setRateA(parser.expand<double>(nd["rate_A"]));
        }
        if (ndRateB)
        {
            cBranch->setRateB(parser.expand<double>(nd["rate_B"]));
        }
        if (ndRateC)
        {
            cBranch->setRateC(parser.expand<double>(nd["rate_C"]));
        }

        return cBranch;
    }
}
