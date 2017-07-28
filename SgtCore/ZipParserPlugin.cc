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

#include "ZipParserPlugin.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"
#include "YamlSupport.h"

namespace Sgt
{
    void ZipParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto zip = parseZip(nd, parser);

        assertFieldPresent(nd, "bus_id");

        std::string busId = parser.expand<std::string>(nd["bus_id"]);
        netw.addZip(std::move(zip), busId);
    }

    std::unique_ptr<Zip> ZipParserPlugin::parseZip(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases");

        std::string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);

        std::unique_ptr<Zip> zip(new Zip(id, phases));

        auto ndYConst = nd["Y_const"];
        auto ndIConst = nd["I_const"];
        auto ndSConst = nd["S_const"];

        if (ndYConst)
        {
            zip->setYConst(parser.expand<arma::Mat<Complex>>(nd["Y_const"]));
        }
        if (ndIConst)
        {
            zip->setIConst(parser.expand<arma::Mat<Complex>>(nd["I_const"]));
        }
        if (ndSConst)
        {
            zip->setSConst(parser.expand<arma::Mat<Complex>>(nd["S_const"]));
        }

        return zip;
    }

}
