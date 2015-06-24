#include "SimBus.h"

namespace Sgt
{
    void SimBusAbc::joinNetwork(SimNetwork& simNetwork)
    {
        auto& network = *simNetwork.network();
        auto networkBus = network.bus(id());
        if (!networkBus)
        {
            network.addBus(bus());
        }
        else
        {
            sgtAssert(networkBus == bus(), "Bus " << id() << " already exists in the network.");
        }

        simNetwork.dependsOn(shared<SimComponent>());

        bus()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        bus()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
    }
            
    void SimBusAbc::initializeState()
    {
        bool sv = bus()->voltageUpdated().isEnabled();
        bus()->voltageUpdated().setIsEnabled(false);
        bus()->setV(bus()->VNom());
        bus()->voltageUpdated().setIsEnabled(sv);
    }
}
