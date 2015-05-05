#include "PvInverter.h"

#include <SgtSim/SimNetwork.h>
#include <SgtSim/Simulation.h>

namespace Sgt
{
    void PvInverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto inverter = sim.newSimComponent<PvInverter>(id, busId);

        if (nd["efficiency"])
        {
            inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }
        
        if (nd["max_Q"])
        {
            inverter->maxQ_ = parser.expand<double>(nd["max_Q"]);
        }

        const std::string networkId = parser.expand<std::string>(nd["network_id"]);
        auto network = sim.simComponent<SimNetwork>(networkId);
        network->addGen(inverter, busId);
    }
}
