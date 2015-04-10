#include "UndergroundLine.h"

#include "PowerFlow.h"

namespace Sgt
{
   UndergroundLine::UndergroundLine(
         const std::string& id, double L, bool hasNeutral, const arma::Mat<double>& phaseDist,
         double gmrPhase, double resPerLPhase,
         double gmrConcStrand, double resPerLConcStrand, int nConcStrand, 
         double rConc, double rhoEarth, double freq) :
      BranchAbc(id, Phase::A|Phase::B|Phase::C, Phase::A|Phase::B|Phase::C),
      L_(L),
      hasNeutral_(hasNeutral),
      phaseDist_(phaseDist),
      gmrPhase_(gmrPhase),
      resPerLPhase_(resPerLPhase),
      gmrConcStrand_(gmrConcStrand),
      resPerLConcStrand_(resPerLConcStrand),
      rhoEarth_(rhoEarth),
      freq_(freq)
   {
      int nPhase = hasNeutral ? 4 : 3; // Not including shielding layers.
      if (phaseDist.n_rows != nPhase || phaseDist.n_cols != nPhase)
      {
         Log().fatal() << "UndergroundLine : distance matrix must be size " << std::to_string(nPhase) << " x " 
                       << std::to_string(nPhase) << "." << std::endl;
      }

      validate(); // TODO: currently, can't adjust any parameters.
   }

   void UndergroundLine::validate()
   {
      int nPhase = phases0().size();
      int nCond = hasNeutral_ ? 7 : 6;

      // Calculate the distance / GMR matrix Dij.
      arma::Mat<double> Dij(nCond, nCond, arma::fill::zeros);
      double nConcStrInv = 1.0 / nConcStrands_;
      double gmrConc = std::pow(gmrConcStrand_ * nConcStrands_ * std::pow(rConc_, nConcStrands_ - 1), nConcStrInv);
      for (int i = 0; i < 3; ++i)
      {
         Dij(i, i) = gmrPhase_; // phase_i - phase_i
         Dij(i + 3, i + 3) = gmrConc; // conc_i - conc_i
         Dij(i, i + 3) = Dij(i + 3, i) = rConc_; // phase_i - conc_i 
         if (hasNeutral_)
         {
            Dij(7, i) = Dij(i, 7) = phaseDist_(4, i); // phase_i - neutral
            Dij(7, i + 3) = Dij(i + 3, 7) = pow(pow(Dij(4, i), nConcStrands_) - pow(rConc_, nConcStrands_),
                                                  nConcStrInv); // conc_i - neutral
         }
         for (int j = 0; j < i; ++j)
         {
            Dij(i, j) = Dij(j, i) = phaseDist_(i, j); // phase_i - phase_j
            Dij(i + 3, j + 3) = Dij(j + 3, i + 3) = phaseDist_(i, j); // conc_i - conc_j
            Dij(i, j + 3) = Dij(j + 3, i) = pow(pow(Dij(i, j), nConcStrands_) - pow(rConc_, nConcStrands_),
                                                  nConcStrInv); // phase_i - conc_j
         }
      }
      if (hasNeutral_)
      {
         Dij(7, 7) = gmrPhase_; // neutral - neutral
      }

      // Calculate the resistance per unit length.
      arma::Col<double> resPerL(nCond);
      double resPerLConc = resPerLConcStrand_ / nConcStrands_;
      for (int i = 0; i < 3; ++i)
      {
         resPerL(i) = resPerLPhase_;
         resPerL(i + 3) = resPerLConc;
      }
      if (hasNeutral_)
      {
         resPerL(7) = resPerLPhase_;
      }

      // Calculate the internal Z matrix (i.e. before Kron)
      arma::Mat<Complex> ZInternal(nCond, nCond, arma::fill::zeros);
      double freqCoeffReal = 9.869611e-7 * freq_;
      double freqCoeffImag = 1.256642e-6 * freq_;
      double freqAdditiveTerm = 0.5 * log(rhoEarth_ / freq_) + 6.490501;
      for (int i = 0; i < nCond; ++i)
      {
         ZInternal(i, i) = {resPerL(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < nCond; ++k)
         {
				ZInternal(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, k)) + freqAdditiveTerm)};
				ZInternal(k, i) = ZInternal(i, k);
         }
      }
      ZInternal *= L_; // Z has been checked against example in Kersting and found to be OK.
      
      // Calculate the external Z matrix (i.e. after Kron)
      arma::Mat<Complex> ZExternal = kron(ZInternal, nPhase);

      // And the line admittance matrix
      arma::Mat<Complex> YLine = arma::inv(ZExternal);

      // And the nodal admittance matrix
      YNode_ = arma::Mat<Complex>(2 * nPhase, 2 * nPhase, arma::fill::zeros);
      for (int i = 0; i < nPhase; ++i)
      {
         for (int j = 0; j < nPhase; ++j)
         {
            YNode_(i, j) = YLine(i, j);
            YNode_(i, j + nPhase) = -YLine(i, j);
            YNode_(i + nPhase, j) = -YLine(i, j);
            YNode_(i + nPhase, j + nPhase) = YLine(i, j);
         }
      }
   }
}
