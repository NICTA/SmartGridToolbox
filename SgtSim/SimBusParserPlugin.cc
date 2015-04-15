#include "SimBusParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Bus.h>
#include <SgtCore/BusParserPlugin.h>

namespace Sgt
{
    void SimBusParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        BusParserPlugin busParser;
        auto bus = sim.newSimComponent<SimBus>(*busParser.parseBus(nd, parser));

        assertFieldPresent(nd, "network_id");
        string netwId = parser.expand<std::string>(nd["network_id"]);
        auto netw = sim.simComponent<SimNetwork>(netwId);
        netw->addBus(bus);
    }
}
