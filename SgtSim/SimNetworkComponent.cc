#include "SimNetworkComponent.h"

namespace SmartGridToolbox
{
   template class SimBranch<BranchAbc>;
   template class SimBus<Bus>;
   template class SimGen<GenAbc>;
   template class SimZip<ZipAbc>;
  
   template class SimBranch<CommonBranch>;
   template class SimBranch<GenericBranch>;
   template class SimBranch<DgyTransformer>;
   template class SimBranch<OverheadLine>;
   template class SimBranch<SinglePhaseTransformer>;

   template class SimGen<GenericGen>;

   template class SimZip<GenericZip>;
}
