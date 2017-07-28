#include "SimZip.h"

namespace Sgt
{
    void link(const ConstSimComponentPtr<SimZip>& simZip, SimNetwork& simNetwork)
    {
        // Safety check that my zip has already been added to simNetwork's network.
        auto networkZip = simNetwork.network().zips()[simZip->zip().id()];
        sgtAssert(networkZip != nullptr, "My Zip must be added to the SimNetwork's Network before calling "
                << __PRETTY_FUNCTION__);
        
        simNetwork.dependsOn(simZip, false);

        simNetwork.needsUpdate().addTrigger(simZip->zip().injectionChanged());
        simNetwork.needsUpdate().addTrigger(simZip->zip().setpointChanged());
        simNetwork.needsUpdate().addTrigger(simZip->zip().isInServiceChanged());
    }
}
