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
        
        SimComponent::addDependency(simBus, simNetwork, false);
        simBus->bus().setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        simBus->bus().isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
