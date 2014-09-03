#ifndef SIM_GEN_DOT_H
#define SIM_GEN_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Gen.h>

namespace SmartGridToolbox
{
   class SimGenAbc : public SimComponentAbc, public GenAbc
   {
      public:
         SimGenAbc(const std::string& id, Phases phases) : GenAbc(id, phases) {}
   };
}

#endif // SIM_GEN_DOT_H
