#include "PowerFlow.h"

#include <iostream>
#include <sstream>

namespace Sgt
{
   namespace
   {
      const int nPhases = 9;

      Phase allPhases[nPhases + 1] =
      {
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
   }

   std::string to_string(BusType type)
   {
      switch (type)
      {
         case BusType::SL:
            return "SL";
            break;
         case BusType::PQ:
            return "PQ";
            break;
         case BusType::PV:
            return "PV";
            break;
         case BusType::NA:
            return "NA";
            break;
         case BusType::BAD:
            return "BAD";
            break;
      }
   }

   template<> BusType from_string<BusType>(const std::string& str)
   {
      static BusType busTypes[] = {BusType::SL, BusType::PQ, BusType::PV, BusType::BAD};
      BusType result = BusType::BAD;
      for (BusType* test = &busTypes[0]; *test != BusType::BAD; ++test)
      {
         if (str == to_string(*test))
         {
            result = *test;
         }
      }
      return result;
   }

   std::string to_string(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL:
            return "BAL";
            break;
         case Phase::A:
            return "A";
            break;
         case Phase::B:
            return "B";
            break;
         case Phase::C:
            return "C";
            break;
         case Phase::G:
            return "G";
            break;
         case Phase::N:
            return "N";
            break;
         case Phase::SP:
            return "SP";
            break;
         case Phase::SM:
            return "SM";
            break;
         case Phase::SN:
            return "SN";
            break;
         case Phase::BAD:
            return "BAD";
            break;
      }
   }

   template<> Phase from_string<Phase>(const std::string& str)
   {
      static Phase phases[] = {Phase::BAL, Phase::A, Phase::B, Phase::C, Phase::G, Phase::N, Phase::SP,
                               Phase::SM, Phase:: SN, Phase::BAD
                              };
      Phase result = Phase::BAD;
      for (Phase* test = &phases[0]; *test != Phase::BAD; ++test)
      {
         if (str == to_string(*test))
         {
            result = *test;
         }
      }
      return result;
   }

   const char* phaseDescr(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL:
            return "balanced/1-phase";
            break;
         case Phase::A:
            return "3-phase A";
            break;
         case Phase::B:
            return "3-phase B";
            break;
         case Phase::C:
            return "3-phase C";
            break;
         case Phase::G:
            return "ground";
            break;
         case Phase::N:
            return "neutral";
            break;
         case Phase::SP:
            return "split-phase +ve";
            break;
         case Phase::SM:
            return "split-phase -ve";
            break;
         case Phase::SN:
            return "split-phase neutral";
            break;
         case Phase::BAD:
            return "UNDEFINED";
            break;
      }
   }

   Phases& Phases::operator&=(const Phases& other)
   {
      mask_ &= other;
      rebuild();
      return *this;
   }

   Phases& Phases::operator|=(const Phases& other)
   {
      mask_ |= other;
      rebuild();
      return *this;
   }

   std::string Phases::to_string() const
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

   arma::Mat<Complex> carson(int nWire, const arma::Mat<double>& Dij, const arma::Col<double> resPerL,
                             double L, double freq, double rhoEarth)
   {
      // Calculate the primative Z matrix (i.e. before Kron)
      arma::Mat<Complex> Z(nWire, nWire, arma::fill::zeros);
      double freqCoeffReal = 9.869611e-7 * freq;
      double freqCoeffImag = 1.256642e-6 * freq;
      double freqAdditiveTerm = 0.5 * log(rhoEarth / freq) + 6.490501;
      for (int i = 0; i < nWire; ++i)
      {
         Z(i, i) = {resPerL(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < nWire; ++k)
         {
            Z(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, k)) + freqAdditiveTerm)};
            Z(k, i) = Z(i, k);
         }
      }
      Z *= L; // Z has been checked against example in Kersting and found to be OK.

      return Z;
   }

   arma::Mat<Complex> kron(const arma::Mat<Complex>& Z, int nPhase)
   {
      int n = Z.n_rows;
      auto Zpp = Z.submat(0, 0, nPhase - 1, nPhase - 1);
      auto Zpn = Z.submat(0, nPhase, nPhase - 1, n - 1);
      auto Znp = Z.submat(nPhase, 0, n - 1, nPhase - 1);
      auto Znn = Z.submat(nPhase, nPhase, n - 1, n - 1);
      auto ZnnInv = arma::inv(Znn);

      return (Zpp - Zpn * ZnnInv * Znp);
   }

   arma::Mat<Complex> ZLine2YNode(const arma::Mat<Complex>& ZLine)
   {
      int n = ZLine.n_rows;

      // The line admittance matrix
      arma::Mat<Complex> YLine = arma::inv(ZLine);

      arma::Mat<Complex>YNode(2 * n, 2 * n, arma::fill::zeros);
      for (int i = 0; i < n; ++i)
      {
         for (int j = 0; j < n; ++j)
         {
            YNode(i, j) = YLine(i, j);
            YNode(i, j + n) = -YLine(i, j);
            YNode(i + n, j) = -YLine(i, j);
            YNode(i + n, j + n) = YLine(i, j);
         }
      }

      return YNode;
   }
}
