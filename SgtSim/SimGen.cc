#include "SimGen.h"

namespace Sgt
{
    void SimGenAbc::linkToSimNetwork(SimNetwork& simNetwork, const std::string& busId)
    {
        // Safety check that my gen has already been added to simNetwork's network.
        auto networkGen = simNetwork.network()->gen(gen()->id());
        sgtAssert(networkGen != nullptr, "My GenAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);

        simNetwork.dependsOn(shared<SimComponent>());

        gen()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        gen()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
    }
}
