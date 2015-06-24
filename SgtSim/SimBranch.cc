#include "SimBranch.h"

namespace Sgt
{
    void SimBranchAbc::joinNetwork(SimNetwork& simNetwork, const std::string& bus0Id, const std::string& bus1Id)
    {
        auto& network = *simNetwork.network();
        auto networkBranch = network.branch(id());
        if (!networkBranch)
        {
            network.addBranch(branch(), bus0Id, bus1Id);
        }
        else
        {
            sgtAssert(networkBranch == branch(), "Branch " << id() << " already exists in the network.");
        }

        simNetwork.dependsOn(shared<SimComponent>());

        branch()->admittanceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
        branch()->isInServiceChanged().addAction([&simNetwork]() {simNetwork.needsUpdate().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
    }
}
