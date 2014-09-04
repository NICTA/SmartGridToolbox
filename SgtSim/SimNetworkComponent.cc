#include "SimNetworkComponent.h"

namespace SmartGridToolbox
{
   template class SimNetworkComponent<BranchAbc>;
   template class SimNetworkComponent<Bus>;
   template class SimNetworkComponent<GenAbc>;
   template class SimNetworkComponent<ZipAbc>;
   
   template class SimNetworkComponentDerived<SimBranchAbc, CommonBranch>;
   template class SimNetworkComponentDerived<SimBranchAbc, GenericBranch>;
   template class SimNetworkComponentDerived<SimBranchAbc, DgyTransformer>;
   template class SimNetworkComponentDerived<SimBranchAbc, OverheadLine>;
   template class SimNetworkComponentDerived<SimGenAbc, GenericGen>;
   template class SimNetworkComponentDerived<SimZipAbc, GenericZip>;
}
