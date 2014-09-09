#include "SimNetwork.h"
#include "SimNetworkComponent.h"


namespace SmartGridToolbox
{
   void SimNetwork::updateState(Time t)
   {
      SGT_DEBUG(debug() << "SimNetwork : update state." << std::endl);
      network_->solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }
}
