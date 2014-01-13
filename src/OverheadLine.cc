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
      if (nNeutral != 0)
      {
         error() << "OverheadLine : internal neutrals not yet implemented." << std::endl;
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
      int n = phases0().size() + nNeutral_;

		double freqCoeffReal = 9.869611e-7 * f_;
		double freqCoeffImag = 1.256642e-6 * f_;
		double freqAdditiveTerm = 0.5 * log(rhoEarth_ / f_) + 6.490501;

      ublas::matrix<Complex> z(n, n, czero);
      for (int i = 0; i < n; ++i)
      {
         z(i, i) = {rhoLine_(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < n; ++k)
         {
				z(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij_(i, k)) + freqAdditiveTerm)};
				z(k, i) = z(i, k);
         }
      }
      z *= L_; // z has been checked against example in Kersting and found to be OK.
      SGT_DEBUG(
            for (int i = 0; i < z.size1(); ++i)
            {
               message() << "z(" << i << ", :) = " << row(z, i) << std::endl;
               message() << std::endl;
            });

      // TODO: eliminate the neutral phase, as per calculation of b_mat in gridLAB-D overhead_line.cpp.
      // Very easy, but interface needs consideration. Equation is:
		// b_mat[0][0] = (z_aa - z_an * z_an * z_nn_inv) * miles;
      // b_mat[0][1] = (z_ab - z_an * z_bn * z_nn_inv) * miles;
      // etc.
      
      ublas::matrix<Complex> y(n, n); bool ok = invertMatrix(z, y); assert(ok);
      SGT_DEBUG(
            for (int i = 0; i < y.size1(); ++i)
            {
               message() << "y(" << i << ", :) = " << row(y, i) << std::endl;
               message() << std::endl;
            });
      
      ublas::matrix<Complex> YNew(2 * n, 2 * n, czero);
      for (int i = 0; i < n; ++i)
      {
         YNew(i, i) += y(i, i);
         YNew(i + n, i + n) += y(i, i); 

         YNew(i, i + n) = -y(i, i); 
         YNew(i + n, i) = -y(i, i); 

         for (int k = i + 1; k < n; ++k)
         {
            // Diagonal terms in node admittance matrix.
            YNew(i, i)         += y(i, k);
            YNew(k, k)         += y(k, i);
            YNew(i + n, i + n) += y(i, k);
            YNew(k + n, k + n) += y(k, i);

            YNew(i, k + n)      = -y(i, k);
            YNew(i + n, k)      = -y(i, k);
            YNew(k, i + n)      = -y(k, i);
            YNew(k + n, i)      = -y(k, i);
         }
      }
      setY(YNew);
   }
}
