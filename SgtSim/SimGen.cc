#include "SimGen.h"

namespace Sgt
{
    void link(const ConstSimComponentPtr<SimGenAbc>& simGen, SimNetwork& simNetwork)
    {
        // Safety check that my gen has already been added to simNetwork's network.
        auto networkGen = simNetwork.network().gens()[simGen->gen().id()];
        sgtAssert(networkGen != nullptr, "My GenAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);

        simNetwork.dependsOn(simGen, false);

        simGen->gen().setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        simGen->gen().isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
