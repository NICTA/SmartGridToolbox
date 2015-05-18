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

#include "BusParserPlugin.h"

#include "Bus.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void BusParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto bus = parseBus(nd, parser);
        netw.addBus(std::move(bus));
    }

    std::unique_ptr<Bus> BusParserPlugin::parseBus(const YAML::Node& nd, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases");
        assertFieldPresent(nd, "type");
        assertFieldPresent(nd, "V_base");
        assertFieldPresent(nd, "V_nom");

        std::string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);
        BusType type = parser.expand<BusType>(nd["type"]);
        double VBase = parser.expand<double>(nd["V_base"]);
        arma::Col<Complex> VNom = parser.expand<arma::Col<Complex>>(nd["V_nom"]);

        std::unique_ptr<Bus> bus(new Bus(id, phases, VNom, VBase));
        bus->setType(type);
        return bus;
    }
}
