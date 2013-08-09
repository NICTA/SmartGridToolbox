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

   constexpr const char * busTypeStr(BusType type)
   {
      switch (type)
      {
         case SL: return "SL"; break;
         case PQ: return "PQ"; break;
         case PV: return "PV"; break;
         case BAD: return "UNDEFINED"; break;
      }
   }

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

   constexpr const char * phaseStr(Phase phase)
   {
      switch (phase)
      {
         case BAL: return "balanced/1-phase"; break;
         case A: return "3-phase A"; break;
         case B: return "3-phase B"; break;
         case C: return "3-phase C"; break;
         case G: return "ground"; break;
         case N: return "neutral"; break;
         case SP: return "split-phase +ve"; break;
         case SM: return "split-phase -ve"; break;
         case SN: return "split-phase neutral"; break;
         case BAD: return "UNDEFINED"; break;
      }
   }
}

#endif // POWERFLOW_DOT_H
