#include "SimBranch.h"

namespace Sgt
{
    void link(const ConstSimComponentPtr<SimBranchAbc>& simBranch, SimNetwork& simNetwork)
    {
        // Safety check that my branch has already been added to simNetwork's network.
        auto networkBranch = simNetwork.network().branches()[simBranch->branch().id()];
        sgtAssert(networkBranch != nullptr, "My BranchAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        simNetwork.dependsOn(simBranch, false);

        simBranch->branch().admittanceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        simBranch->branch().isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
