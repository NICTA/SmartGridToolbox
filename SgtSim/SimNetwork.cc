#include "SimNetwork.h"
#include "SimNetworkComponent.h"


namespace Sgt
{
   void SimNetwork::updateState(Time t)
   {
      SGT_DEBUG(Log().debug() << "SimNetwork : update state." << std::endl);
      network_->solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }
}
