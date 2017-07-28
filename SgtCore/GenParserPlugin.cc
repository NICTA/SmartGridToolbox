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

#include "GenParserPlugin.h"

#include "Bus.h"
#include "Gen.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void GenParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto gen = parseGen(nd, parser);

        assertFieldPresent(nd, "bus_id");
        std::string busId = parser.expand<std::string>(nd["bus_id"]);
        netw.addGen(std::move(gen), busId);
    }

    std::unique_ptr<Gen> GenParserPlugin::parseGen(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases");

        std::string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);

        std::unique_ptr<Gen> gen(new Gen(id, phases));

        if (const YAML::Node& subNd = nd["S"])
        {
            gen->setInServiceS(parser.expand<arma::Col<Complex>>(subNd));
        }

        if (const YAML::Node& subNd = nd["P_min"])
        {
            gen->setPMin(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["P_max"])
        {
            gen->setPMax(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["Q_min"])
        {
            gen->setQMin(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["Q_max"])
        {
            gen->setQMax(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["J"])
        {
            gen->setInServiceJ(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["C0"])
        {
            gen->setC0(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["C1"])
        {
            gen->setC1(parser.expand<double>(subNd));
        }

        if (const YAML::Node& subNd = nd["C2"])
        {
            gen->setC2(parser.expand<double>(subNd));
        }

        return gen;
    }
}
