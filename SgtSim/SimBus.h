#ifndef SIM_BUS_DOT_H
#define SIM_BUS_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Bus.h>

namespace SmartGridToolbox
{
   class SimBus : public SimComponentAbc, public Bus
   {
      public:
         SimBus(const std::string& id, Phases phases, const ublas::vector<Complex>& VNom, double VBase) :
            Bus(id, phases, VNom, VBase) {}
   };
}

#endif // SIM_BUS_DOT_H
