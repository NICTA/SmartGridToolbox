#include "OverheadLine.h"

namespace SmartGridToolbox
{
   OverheadLine::OverheadLine(const std::string& name, const Phases& phases0, const Phases& phases1, double length,
                              int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                              ublas::matrix<double> distMat, double freq)
      : SimBranch(name, phases0, phases1),
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

		double freqCoeffReal = 9.869611e-7*f_;
		double freqCoeffImag = 1.256642e-6*f_;
		double freqAdditiveTerm = 0.5*log(rhoEarth_/f_) + 6.490501;

      ZWire_ = ublas::matrix<Complex>(nWire, nWire, czero);
      for (int i = 0; i < nWire; ++i)
      {
         ZWire_(i, i) = {rhoLine_(i) + freqCoeffReal, freqCoeffImag*(log(1/Dij_(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < nWire; ++k)
         {
				ZWire_(i, k) = {freqCoeffReal, freqCoeffImag*(log(1/Dij_(i, k)) + freqAdditiveTerm)};
				ZWire_(k, i) = ZWire_(i, k);
         }
      }
      ZWire_ *= L_; // Z has been checked against example in Kersting and found to be OK.
      SGT_DEBUG(
            message() << "Before Kron:" << std::endl;
            for (int i = 0; i < ZWire_.size1(); ++i)
            {
               message() << "Z(" << i << ", :) = " << row(ZWire_, i) << std::endl;
               message() << std::endl;
            });

      ZPhase_ = project(ZWire_, ublas::range(0, nPhase), ublas::range(0, nPhase));

      // Apply Kron reduction to eliminate neutral.
      if (nNeutral_ == 1)
      {
         int iNeutral = nPhase;
         Complex ZnnInv = 1.0/ZWire_(iNeutral, iNeutral); // Assuming only one neutral!
         for (int i = 0; i < nPhase; ++i)
         {
            for (int j = 0; j < nPhase; ++j)
            {
               ZPhase_(i, j) -= ZWire_(i, iNeutral)*ZWire_(iNeutral, j)*ZnnInv;
            }
         }
      }

      SGT_DEBUG(
            message() << "After Kron:" << std::endl;
            for (int i = 0; i < ZPhase_.size1(); ++i)
            {
               message() << "Z(" << i << ", :) = " << row(ZPhase_, i) << std::endl;
               message() << std::endl;
            });

      ublas::matrix<Complex> Y(nPhase, nPhase);
      bool ok = invertMatrix(ZPhase_, Y); assert(ok);
      SGT_DEBUG(
            for (int i = 0; i < Y.size1(); ++i)
            {
               message() << "Y(" << i << ", :) = " << row(Y, i) << std::endl;
               message() << std::endl;
            });

      ublas::matrix<Complex> YNode(2*nPhase, 2*nPhase, czero);
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
      SGT_DEBUG(
            for (int i = 0; i < YNode.size1(); ++i)
            {
               message() << "YNode(" << i << ", :) = " << row(YNode, i) << std::endl;
               message() << std::endl;
            });
      setY(YNode);
   }
}
