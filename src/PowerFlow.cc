#include "PowerFlow.h"

namespace SmartGridToolbox
{
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

   BusType str2BusType(const std::string & str)
   {
      static BusType busTypes[] = {BusType::SL, BusType::PQ, BusType::PV, BusType::BAD};
      BusType result = BusType::BAD;
      for (BusType * test = &busTypes[0]; *test != BusType::BAD; ++test)
      {
         if (str == busTypeStr(*test))
         {
            result = *test; 
         }
      }
      return result;
   }

   const char * phase2Str(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL: return "BAL"; break;
         case Phase::A: return "A"; break;
         case Phase::B: return "B"; break;
         case Phase::C: return "C"; break;
         case Phase::G: return "G"; break;
         case Phase::N: return "N"; break;
         case Phase::SP: return "SP"; break;
         case Phase::SM: return "SM"; break;
         case Phase::SN: return "SN"; break;
         case Phase::BAD: return "BAD"; break;
      }
   }

   Phase str2Phase(const std::string & str)
   {
      static Phase phases[] = {Phase::BAL, Phase::A, Phase::B, Phase::C, Phase::G, Phase::N, Phase::SP, 
                               Phase::SM, Phase:: SN, Phase::BAD};
      Phase result = Phase::BAD;
      for (Phase * test = &phases[0]; *test != Phase::BAD; ++test)
      {
         if (str == phase2Str(*test))
         {
            result = *test; 
         }
      }
      return result;
   }

   const char * phaseDescr(Phase phase)
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
