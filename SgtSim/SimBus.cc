#include "SimBus.h"

namespace Sgt
{
    void SimBusAbc::initializeState()
    {
        bool sv = bus().voltageUpdated().isEnabled();
        bus().voltageUpdated().setIsEnabled(false);
        bus().setV(bus().VNom());
        bus().voltageUpdated().setIsEnabled(sv);
    }

    void link(const ConstSimComponentPtr<SimBusAbc>& simBus, SimNetwork& simNetwork)
    {
        // Safety check that my bus has already been added to simNetwork's network.
        auto networkBus = simNetwork.network().buses()[simBus->bus().id()];
        sgtAssert(networkBus != nullptr, "My Bus must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        simNetwork.dependsOn(simBus, false);

        simNetwork.needsUpdate().addTrigger(simBus->bus().setpointChanged());
        simNetwork.needsUpdate().addTrigger(simBus->bus().isInServiceChanged());
    }
}
