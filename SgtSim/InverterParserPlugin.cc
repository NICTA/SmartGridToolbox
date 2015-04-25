#include "InverterParserPlugin.h"

#include "Inverter.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace Sgt
{
    void InverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases");
        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);

        auto inverter = sim.newSimComponent<Inverter>(id, phases);

        if (nd["efficiency"])
        {
            inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }

        if (nd["min_power_factor"])
        {
            inverter->setMinPowerFactor(parser.expand<double>(nd["min_power_factor"]));
        }

        if (nd["requested_Q"])
        {
            inverter->setRequestedQ(parser.expand<double>(nd["requested_Q"]));
        }

        const std::string networkId = parser.expand<std::string>(nd["network_id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto network = sim.simComponent<SimNetwork>(networkId);
        network->addZip(inverter, busId);
    }
}
