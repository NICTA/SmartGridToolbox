#include "NRLoadFlow.h"

namespace SmartGridToolbox
{
   void NRLoadFlow::addBus(NRBus * bus);
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

   void NRLoadFlow::validate()
   {
      nSL_ = size(SLBusses_);
      nPQ_ = size(PQBusses_);

      assert(nSL_ == 1); // May change in future...
      assert(nPQ_ > 0); // May change in future...

      busses = BusVec(); // Clear contents...
      busses.reserve(nSL_ + nPQ_);
      busses_.insert(busses_.end(), SLBusses.begin(), SLBusses.end());
      busses_.insert(busses_.end(), PQBusses.begin(), PQBusses.end());

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
         
      buildBusAdmit();
   }

   void NRLoadFlow::buildBusAdmit()
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

   void NRLoadFlow::initx()
   {
      x_.resize(3 * nPQ_);

      for (int i = 0; i < 3 * nPQ_; ++i)
      {
         const NRBus & bus = *busses[i + 1]; 
         x_(i) = V0_[i].real();
         x_(i + nPQ_) = V0_[i].imag();
      }
   }

   void NRLoadFlow::updateF()
   {
      int ix1 = 1;
      int ix2 = nPQ_;
      int ix3 = i2 + 1;
      int ix4 = 2 * nPQ_;


      Vr = [x(i1:i2);real(bus.V(bus.iSL))];
      Vi = [x(i3:i4);imag(bus.V(bus.iSL))];
      M2 = Vr(bus.iPQ).^2+Vi(bus.iPQ).^2;
      DR = (-PPQ.*x(i1:i2) - QPQ.*x(i3:i4))./M2 + G(bus.iPQ,:) * Vr - B(bus.iPQ,:) * Vi;
      DI = (-PPQ.*x(i3:i4) + QPQ.*x(i1:i2))./M2 + G(bus.iPQ,:) * Vi + B(bus.iPQ,:) * Vr;
      f = [DR; DI];
   }

   void NRLoadFlow::solve()
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
