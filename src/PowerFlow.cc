#include "PowerFlow.h"

#include <iostream>
#include <sstream>

namespace SmartGridToolbox
{
   static const int nPhases = 9;
   static Phase allPhases[nPhases + 1] = {
      Phase::BAL, 
      Phase::A, 
      Phase::B, 
      Phase::C, 
      Phase::G, 
      Phase::N, 
      Phase::SP, 
      Phase::SM, 
      Phase::SN,
      Phase::BAD
   };

   const char * busType2Str(BusType type)
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
         if (str == busType2Str(*test))
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

   Phases & Phases::operator&=(const Phases & other)
   {
      mask_ &= other;
      rebuild();
      return *this;
   }

   Phases & Phases::operator|=(const Phases & other)
   {
      mask_ |= other;
      rebuild();
      return *this;
   }

   std::string Phases::toStr() const
   {
      std::ostringstream ss;
      ss << phaseVec_[0];
      for (int i = 1; i < phaseVec_.size(); ++i)
      {
         ss << "|" << phaseVec_[i];
      }
      return ss.str();
   }

   void Phases::rebuild()
   {
      phaseVec_ = PhaseVec();
      phaseVec_.reserve(nPhases);
      idxMap_ = IdxMap();
      for (unsigned int i = 0, j = 0; allPhases[i] != Phase::BAD; ++i)
      {
         if (hasPhase(allPhases[i]))
         {
            phaseVec_.push_back(allPhases[i]);
            idxMap_[allPhases[i]] = j++;
         }
      }
      phaseVec_.shrink_to_fit();
   }

   // Balanced/1-phase simple line with a single admittance.
   const UblasMatrix<Complex> YLine1P(const Complex & y)
   {
      UblasMatrix<Complex> Y(2, 2);
      Y(0, 0) = y;
      Y(1, 1) = y;
      Y(0, 1) = -y;
      Y(1, 0) = -y;
      return Y;
   }

   // No cross terms, just nPhase lines with single admittances.
   const UblasMatrix<Complex> YSimpleLine(const UblasVector<Complex> & y)
   {
      int nPhase = y.size();
      int nTerm = 2 * nPhase; 
      UblasMatrix<Complex> Y(nTerm, nTerm);
      for (int i = 0; i < nPhase; ++i)
      {
         Y(i, i) = y(i);
         Y(i + nPhase, i + nPhase) = y(i);
         Y(i, i + nPhase) = -y(i);
         Y(i + nPhase, i) = -y(i);
      }
      return Y;
   }
}
