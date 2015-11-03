#include "SimGen.h"

namespace Sgt
{
    void SimGenAbc::linkToSimNetwork(SimNetwork& simNetwork)
    {
        // Safety check that my gen has already been added to simNetwork's network.
        auto networkGen = simNetwork.network()->gen(gen()->id());
        sgtAssert(networkGen != nullptr, "My GenAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);

        simNetwork.dependsOn(this);

        gen()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        gen()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
