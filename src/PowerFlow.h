#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

namespace SmartGridToolbox
{
   enum class BusType
   {
      SL,
      PQ,
      PV,
      BAD
   };

   const char * busTypeStr(BusType type);

   enum class Phase
   {
      BAL,  // Balanced/one-phase.
      A,    // Three phase A.
      B,    // Three phase B.
      C,    // Three phase C.
      G,    // Ground.
      N,    // Neutral, to be used only when it is distinct from ground.
      SP,   // Split phase plus.
      SM,   // Split phase minus.
      SN,   // Split phase neutral.
      BAD
   };

   const char * phaseStr(Phase phase);
}

#endif // POWERFLOW_DOT_H
