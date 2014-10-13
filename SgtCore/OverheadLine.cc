#include "OverheadLine.h"

namespace SmartGridToolbox
{
   OverheadLine::OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double length,
                              int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                              ublas::matrix<double> distMat, double freq) :
      BranchAbc(id, phases0, phases1),
      L_(length),
      nNeutral_(nNeutral),
      rhoLine_(lineResistivity),
      rhoEarth_(earthResistivity),
      Dij_(distMat),
      f_(freq)
   {
      if (nNeutral > 1)
      {
         Log().fatal() << "OverheadLine : currently there must be either zero or one neutral wires." << std::endl;
      }
      int n = phases0.size() + nNeutral;
      if (lineResistivity.size() != n || distMat.size1() != n || distMat.size2() != n)
      {
         Log().fatal() << "OverheadLine : wrong number of internal phases." << std::endl;
      }
   }

   const ublas::matrix<Complex> OverheadLine::Y() const
   {
      int nPhase = phases0().size();

      ublas::matrix<Complex> ZWire = this->ZWire();
      ublas::matrix<Complex> ZPhase = this->ZPhase(ZWire);

      ublas::matrix<Complex> Y(nPhase, nPhase);
      bool ok = invertMatrix(ZPhase, Y); assert(ok);

      ublas::matrix<Complex> YNode(2 * nPhase, 2 * nPhase, czero);
      for (int i = 0; i < nPhase; ++i)
      {
         for (int j = 0; j < nPhase; ++j)
         {
            YNode(i, j) = Y(i, j);
            YNode(i, j + nPhase) = -Y(i, j);
            YNode(i + nPhase, j) = -Y(i, j);
            YNode(i + nPhase, j + nPhase) = Y(i, j);
         }
      }

      return YNode;
   }

   ublas::matrix<Complex> OverheadLine::ZWire() const
   {
      int nPhase = phases0().size();
      int nWire = nPhase + nNeutral_;

      double freqCoeffReal = 9.869611e-7 * f_;
      double freqCoeffImag = 1.256642e-6 * f_;
      double freqAdditiveTerm = 0.5 * log(rhoEarth_ / f_) + 6.490501;

      auto result = ublas::matrix<Complex>(nWire, nWire, czero);
      for (int i = 0; i < nWire; ++i)
      {
         result(i, i) = {rhoLine_(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < nWire; ++k)
         {
				result(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, k)) + freqAdditiveTerm)};
				result(k, i) = result(i, k);
         }
      }
      result *= L_; // Z has been checked against example in Kersting and found to be OK.

      return result;
   }

   ublas::matrix<Complex> OverheadLine::ZPhase(const ublas::matrix<Complex>& ZWire) const
   {
      int nPhase = phases0().size();

      ublas::matrix<Complex> result = project(ZWire, ublas::range(0, nPhase), ublas::range(0, nPhase));

      // Apply Kron reduction to eliminate neutral.
      if (nNeutral_ == 1)
      {
         int iNeutral = nPhase;
         Complex ZnnInv = 1.0 / ZWire(iNeutral, iNeutral); // Assuming only one neutral!
         for (int i = 0; i < nPhase; ++i)
         {
            for (int j = 0; j < nPhase; ++j)
            {
               result(i, j) -= ZWire(i, iNeutral) * ZWire(iNeutral, j) * ZnnInv;
            }
         }
      }

      return result;
   }
}
