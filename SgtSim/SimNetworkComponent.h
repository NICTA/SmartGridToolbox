#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Gen.h>
#include <SgtCore/Zip.h>

namespace SmartGridToolbox
{
   class SimBranchInterface : virtual public SimComponentInterface, virtual public BranchInterface {};
   class SimBusInterface : virtual public SimComponentInterface, virtual public BusInterface {};
   class SimGenInterface : virtual public SimComponentInterface, virtual public GenInterface {};
   class SimZipInterface : virtual public SimComponentInterface, virtual public ZipInterface {};

   class SimGenAbc : public GenAbc, public SimComponent, virtual public SimGenInterface {};
   class SimZipAbc : public ZipAbc, public SimComponent, virtual public SimZipInterface {};
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
