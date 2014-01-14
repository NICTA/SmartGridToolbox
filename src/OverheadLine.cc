#include "OverheadLine.h"

namespace SmartGridToolbox
{
   OverheadLine::OverheadLine(const std::string & name, const Phases & phases0, const Phases & phases1, double length,
                              int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                              ublas::matrix<double> distMat, double freq)
      : Branch(name, phases0, phases1),
        L_(length),
        nNeutral_(nNeutral),
        rhoLine_(lineResistivity),
        rhoEarth_(earthResistivity),
        Dij_(distMat),
        f_(freq)
   {
      if (nNeutral > 1)
      {
         error() << "OverheadLine : currently there must be either zero or one neutral wires." << std::endl;
         abort();
      }
      int n = phases0.size() + nNeutral;
      if (lineResistivity.size() != n || distMat.size1() != n || distMat.size2() != n)
      {
         error() << "OverheadLine : wrong number of internal phases." << std::endl;
         abort();
      }

      recalcY(); 
   }

   void OverheadLine::recalcY()
   {
      int nPhase = phases0().size();
      int nWire = nPhase + nNeutral_;

		double freqCoeffReal = 9.869611e-7 * f_;
		double freqCoeffImag = 1.256642e-6 * f_;
		double freqAdditiveTerm = 0.5 * log(rhoEarth_ / f_) + 6.490501;

      ublas::matrix<Complex> zWire(nWire, nWire, czero);
      for (int i = 0; i < nWire; ++i)
      {
         zWire(i, i) = {rhoLine_(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < nWire; ++k)
         {
				zWire(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, k)) + freqAdditiveTerm)};
				zWire(k, i) = zWire(i, k);
         }
      }
      zWire *= L_; // z has been checked against example in Kersting and found to be OK.

      ublas::matrix<Complex> zPhase = project(zWire, ublas::range(0, nPhase), ublas::range(0, nPhase));

      // Apply Kron reduction to eliminate neutral.
      if (nNeutral_ == 1)
      {
         int iNeutral = nPhase;
         Complex znnInv = 1.0 / zWire(iNeutral, iNeutral); // Assuming only one neutral!
         for (int i = 0; i < nPhase; ++i)
         {
            for (int j = 0; j < nPhase; ++j)
            {
               zPhase(i, j) -= zWire(i, iNeutral) * zWire(iNeutral, j) * znnInv;
            }
         }
      }

      SGT_DEBUG(
            for (int i = 0; i < zPhase.size1(); ++i)
            {
               message() << "z(" << i << ", :) = " << row(zPhase, i) << std::endl;
               message() << std::endl;
            });

      ublas::matrix<Complex> y(nPhase, nPhase);
      bool ok = invertMatrix(zPhase, y); assert(ok);
      SGT_DEBUG(
            for (int i = 0; i < y.size1(); ++i)
            {
               message() << "y(" << i << ", :) = " << row(y, i) << std::endl;
               message() << std::endl;
            });
      
      ublas::matrix<Complex> YNode(2 * nPhase, 2 * nPhase, czero);
      for (int i = 0; i < nPhase; ++i)
      {
         YNode(i, i) += y(i, i);
         YNode(i + nPhase, i + nPhase) += y(i, i); 

         YNode(i, i + nPhase) = -y(i, i); 
         YNode(i + nPhase, i) = -y(i, i); 

         for (int k = i + 1; k < nPhase; ++k)
         {
            // Diagonal terms in node admittance matrix.
            YNode(i, i)         += y(i, k);
            YNode(k, k)         += y(k, i);
            YNode(i + nPhase, i + nPhase) += y(i, k);
            YNode(k + nPhase, k + nPhase) += y(k, i);

            YNode(i, k + nPhase)      = -y(i, k);
            YNode(i + nPhase, k)      = -y(i, k);
            YNode(k, i + nPhase)      = -y(k, i);
            YNode(k + nPhase, i)      = -y(k, i);
         }
      }
      setY(YNode);
   }
}
