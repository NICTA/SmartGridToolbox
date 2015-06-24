#include "SimGen.h"

namespace Sgt
{
    void SimGenAbc::joinNetwork(SimNetwork& simNetwork, const std::string& busId)
    {
        auto& network = *simNetwork.network();
        auto networkGen = network.gen(busId);
        if (!networkGen)
        {
            network.addGen(gen(), busId);
        }
        else
        {
            sgtAssert(networkGen == gen(), "Gen " << id() << " already exists in the network.");
        }

        simNetwork.dependsOn(shared<SimComponent>());

        gen()->setpointChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        gen()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
    }
}
