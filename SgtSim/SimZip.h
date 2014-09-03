#ifndef SIM_ZIP_DOT_H
#define SIM_ZIP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Zip.h>

namespace SmartGridToolbox
{
   class SimZipAbc : public SimComponentAbc, public ZipAbc
   {
      public:
         SimZipAbc(const std::string& id, Phases phases) : ZipAbc(id, phases) {}
   };
}

#endif // SIM_ZIP_DOT_H
