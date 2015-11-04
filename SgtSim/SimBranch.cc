#include "SimBranch.h"

namespace Sgt
{
    void SimBranchAbc::linkToSimNetwork(SimNetwork& simNetwork)
    {
        // Safety check that my branch has already been added to simNetwork's network.
        auto networkBranch = simNetwork.network()->branch(branch()->id());
        sgtAssert(networkBranch != nullptr, "My BranchAbc must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        SimComponent::addDependency(*this, simNetwork, false);

        branch()->admittanceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
        branch()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + simNetwork.componentType() + " " + simNetwork.id() + " needs update");
    }
}
