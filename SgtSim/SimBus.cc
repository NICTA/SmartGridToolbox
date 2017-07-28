#include "SimBus.h"

namespace Sgt
{
    void SimBus::initializeState()
    {
        bool sv = bus_->voltageUpdated().isEnabled();
        bus_->voltageUpdated().setIsEnabled(false);
        bus_->setV(bus_->VNom());
        bus_->voltageUpdated().setIsEnabled(sv);
    }

    void link(const ConstSimComponentPtr<SimBus>& simBus, SimNetwork& simNetwork)
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
