#include <SmartGridToolbox/PowerFlow.h>

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

   const char* busType2Str(BusType type)
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
      for (BusType* test = &busTypes[0]; *test != BusType::BAD; ++test)
      {
         if (str == busType2Str(*test))
         {
            result = *test; 
         }
      }
      return result;
   }

   const char* phase2Str(Phase phase)
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
      for (Phase* test = &phases[0]; *test != Phase::BAD; ++test)
      {
         if (str == phase2Str(*test))
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
   const ublas::matrix<Complex> YLine1P(const Complex & Y)
   {
      ublas::matrix<Complex> YNode(2, 2, czero);
      YNode(0, 0) = Y;
      YNode(1, 1) = Y;
      YNode(0, 1) = -Y;
      YNode(1, 0) = -Y;
      return YNode;
   }

   // No cross terms, just nPhase lines with single admittances.
   const ublas::matrix<Complex> YSimpleLine(const ublas::vector<Complex> & Y)
   {
      int nPhase = Y.size();
      int nTerm = 2*nPhase; 
      ublas::matrix<Complex> YNode(nTerm, nTerm, czero);
      for (int i = 0; i < nPhase; ++i)
      {
         YNode(i, i) = Y(i);
         YNode(i + nPhase, i + nPhase) = Y(i);
         YNode(i, i + nPhase) = -Y(i);
         YNode(i + nPhase, i) = -Y(i);
      }
      return YNode;
   }
   
   ublas::matrix<Complex> YOverheadLine(ublas::vector<double> r, ublas::matrix<double> DMat, double L,
                                        double freq, double rho)
   {
      int n = r.size();
      assert(DMat.size1() == n);
      assert(DMat.size2() == n);

		double freqCoeffReal = 9.869611e-7*freq;
		double freqCoeffImag = 1.256642e-6*freq;
		double freqAdditiveTerm = 0.5*log(rho/freq) + 6.490501;

      ublas::matrix<Complex> Z(n, n, czero);
      for (int i = 0; i < n; ++i)
      {
         Z(i, i) = {r(i) + freqCoeffReal, freqCoeffImag*(log(1/DMat(i, i)) + freqAdditiveTerm)};
         for (int k = i + 1; k < n; ++k)
         {
				Z(i, k) = {freqCoeffReal, freqCoeffImag*(log(1/DMat(i, k)) + freqAdditiveTerm)};
				Z(k, i) = Z(i, k);
         }
      }
      Z *= L; // Z has been checked against example in Kersting and found to be OK.
      SGT_DEBUG(
            for (int i = 0; i < Z.size1(); ++i)
            {
               message() << "Z(" << i << ", :) = " << row(Z, i) << std::endl;
               message() << std::endl;
            });

      // TODO: eliminate the neutral phase, as per calculation of b_mat in gridLAB-D overhead_line.cpp.
      // Very easy, but interface needs consideration. Equation is:
		// b_mat[0][0] = (Z_aa - Z_an*Z_an*Z_nn_inv)*miles;
      // b_mat[0][1] = (Z_ab - Z_an*Z_bn*Z_nn_inv)*miles;
      // etc.
      
      ublas::matrix<Complex> Y(n, n); bool ok = invertMatrix(Z, Y); assert(ok);
      SGT_DEBUG(
            for (int i = 0; i < Y.size1(); ++i)
            {
               message() << "Y(" << i << ", :) = " << row(Y, i) << std::endl;
               message() << std::endl;
            });
      
      ublas::matrix<Complex> YNode(2*n, 2*n, czero);
      for (int i = 0; i < n; ++i)
      {
         YNode(i, i) += Y(i, i);
         YNode(i + n, i + n) += Y(i, i); 

         YNode(i, i + n) = -Y(i, i); 
         YNode(i + n, i) = -Y(i, i); 

         for (int k = i + 1; k < n; ++k)
         {
            // Diagonal terms in node admittance matrix.
            YNode(i, i)         += Y(i, k);
            YNode(k, k)         += Y(k, i);
            YNode(i + n, i + n) += Y(i, k);
            YNode(k + n, k + n) += Y(k, i);

            YNode(i, k + n)      = -Y(i, k);
            YNode(i + n, k)      = -Y(i, k);
            YNode(k, i + n)      = -Y(k, i);
            YNode(k + n, i)      = -Y(k, i);
         }
      }
      std::cout << "Returning " << YNode.size1() << std::endl;

      return YNode;
   }
}
