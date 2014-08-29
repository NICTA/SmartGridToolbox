#ifndef SIM_GEN_DOT_H
#define SIM_GEN_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Gen.h>

namespace SmartGridToolbox
{
   class SimGen : public Gen, public SimComponent
   {
      public:

         SimGen(const std::string& id, Phases phases) : Gen(id, phases) {}
   };
}

#endif // SIM_GEN_DOT_H
