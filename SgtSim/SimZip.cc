#include "SimZip.h"

namespace Sgt
{
    void SimZipAbc::linkToSimNetwork(SimNetwork& simNetwork)
    {
        // Safety check that my zip has already been added to simNetwork's network.
        auto networkZip = simNetwork.network()->zip(zip()->id());
        sgtAssert(networkZip != nullptr, "My ZipAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        simNetwork.dependsOn(shared<SimComponent>());

        zip()->injectionChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        zip()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        zip()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
