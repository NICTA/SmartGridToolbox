#include "PowerFlowNR.h"

namespace SmartGridToolbox
{
   void PowerFlowNR::addBus(NRBus * bus);
   {
      switch (bus.type)
      {
         case BusType::SL :
            SLBusses_.push_back(bus);
            break;
         case BusType::PQ : 
            PQBusses_.push_back(bus);
            break;
         case BusType::PV : 
            Error("PV busses are not supported yet.");
            break;
      }
   }

   void PowerFlowNR::validate()
   {
      nSL_ = size(SLBusses_);
      nPQ_ = size(PQBusses_);
      n_ = nSL_ + nPQ_;

      assert(nSL_ == 1); // May change in future...
      assert(nPQ_ > 0); // May change in future...

      busses = BusVec(); // Clear contents...
      busses.reserve(nPQ_ + nSL_);
      busses_.insert(busses_.end(), PQBusses.begin(), PQBusses.end());
      busses_.insert(busses_.end(), SLBusses.begin(), SLBusses.end());

      for (int i = 0; i < PQBusses_.size(); ++i)
      {
         PQBusses_(i).idxPQ = i;
      }

      V0_ = SLBusses[0].V; // Array copy.

      for (int i = 0; i < nPQ_; ++i)
      {
         for (int k = 0; k < 3; ++k)
         {
            PPQ_(3 * i + k) = Busses[i].P[k];
            QPQ_(3 * i + k) = Busses[i].Q[k];
         }
      }

      Vr_.resize(n_);
      Vi_.resize(n_);
         
      buildBusAdmit();
   }

   void PowerFlowNR::buildBusAdmit()
   {
      for (const NrBranchData * const branch : branches)
      {
         const Bus * busi = branch.busi;
         const Bus * busk = branch.busk;

         int ibus = busi.idxPQ;
         int kbus = busk.idxPQ;

         int idxi = 3 * i;
         int idxk = 3 * k;
         for (int p1 = 0; p1 < 3; ++p1)
         {
            for (int p2 = 0; p2 < 3; ++p2)
            {
               Y_(idxi + p1, idxi + p2) += branch.Y(p1, p2);
               Y_(idxk + p1, idxk + p2) += branch.Y(p1 + 3, p2 + 3);
               Y_(idxi + p1, idxk + p2) += branch.Y(p1, p2 + 3);
               Y_(idxk + p1, idxi + p2) += branch.Y(p1 + 3, p2);
            }
         }
      }
      G_ = real(Y_);
      B_ = imag(Y_);
   }

   void PowerFlowNR::initx()
   {
      x_.resize(3 * nPQ_);

      for (int i = 0; i < 3 * nPQ_; ++i)
      {
         const NRBus & bus = *busses[i + 1]; 
         x_(i) = V0_[i].real();
         x_(i + nPQ_) = V0_[i].imag();
      }
   }

   void PowerFlowNR::updateF()
   {
      using namespace ublas;
      range rPQ(0, nPQ_);
      range rAll(0, nPQ_ + 1);
      range r1(0, nPQ_);
      range r2(nPQ, 2 * nPQ_);

      vector_range x1(x, r1);
      vector_range x2(x, r2);

      vector_range(Vr, rPQ) = vector_range(x, r1);
      Vr(nPQ) = V0.real();
      vector_range(Vi, rPQ) = vector_range(x, r2);
      Vi(nPQ) = V0.imag();

      ublas::vector<double> M2 = element_prod(Vr, Vr) + element_prod(Vi, Vi); 

      matrix_range<matrix<double>> Grng(G, rPQ, rAll); 
      matrix_range<matrix<double>> Brng(G, rPQ, rAll); 

      ublas::vector<double> DR = element_div((-element_prod(PPQ_, x1) - element_prod(QPQ_, x2)), M2)
                               + prod(Grng, Vr) - prod(Brng, Vi);
      ublas::vector<double> DI = element_div((-element_prod(PPQ_, x2) + element_prod(QPQ_, x1)), M2)
                               + prod(Grng, Vi) + prod(Brng, Vr);

      vector_range(f_, r1) = DR;
      vector_range(f_, r2) = DI;
   }

   void PowerFlowNR::solve()
   {
      const double tol = 1e-20;
      const int maxiter = 20;
      initx();
      for (int i = 0; i < maxiter; ++ i)
      {
         calcF

      }
   }
}
