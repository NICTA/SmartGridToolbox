#include "SimBranch.h"

namespace Sgt
{
    void link(const ConstSimComponentPtr<SimBranch>& simBranch, SimNetwork& simNetwork)
    {
        // Safety check that my branch has already been added to simNetwork's network.
        auto networkBranch = simNetwork.network().branches()[simBranch->branch().id()];
        sgtAssert(networkBranch != nullptr, "My Branch must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        simNetwork.dependsOn(simBranch, false);

        simNetwork.needsUpdate().addTrigger(simBranch->branch().admittanceChanged());
        simNetwork.needsUpdate().addTrigger(simBranch->branch().isInServiceChanged());
    }
}
