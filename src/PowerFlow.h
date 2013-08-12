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

   const char * busTypeStr(BusType type)
   {
      switch (type)
      {
         case BusType::SL: return "SL"; break;
         case BusType::PQ: return "PQ"; break;
         case BusType::PV: return "PV"; break;
         case BusType::BAD: return "UNDEFINED"; break;
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

   const char * phaseStr(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL: return "balanced/1-phase"; break;
         case Phase::A: return "3-phase A"; break;
         case Phase::B: return "3-phase B"; break;
         case Phase::C: return "3-phase C"; break;
         case Phase::G: return "ground"; break;
         case Phase::N: return "neutral"; break;
         case Phase::SP: return "split-phase +ve"; break;
         case Phase::SM: return "split-phase -ve"; break;
         case Phase::SN: return "split-phase neutral"; break;
         case Phase::BAD: return "UNDEFINED"; break;
      }
   }
}

#endif // POWERFLOW_DOT_H
