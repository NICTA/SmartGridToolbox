#include "SimGenericGenParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Gen.h>
#include <SgtCore/GenericGenParserPlugin.h>

#include <memory>

namespace Sgt
{
    void SimGenericGenParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        GenericGenParserPlugin ggParser;
        auto gg = sim.newSimComponent<SimGenericGen>(*ggParser.parseGenericGen(nd, parser));

        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");

        string netwId = parser.expand<std::string>(nd["network_id"]);
        std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto netw = sim.simComponent<SimNetwork>(netwId);

        netw->addGen(gg, busId);
    }
}
