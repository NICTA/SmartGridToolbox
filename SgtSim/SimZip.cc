#include "SimZip.h"

namespace Sgt
{
    void SimZipAbc::joinNetwork(SimNetwork& simNetwork, const std::string& busId)
    {
        auto& network = *simNetwork.network();
        auto networkZip = network.zip(id());
        if (!networkZip)
        {
            network.addZip(zip(), busId);
        }
        else
        {
            sgtAssert(networkZip == zip(), "Zip " << id() << " already exists in the network.");
        }

        simNetwork.dependsOn(shared<SimComponent>());

        zip()->injectionChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        zip()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        zip()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
    }
}
