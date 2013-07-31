#include "PowerFlowNR.h"

namespace SmartGridToolbox
{
   void PowerFlowNR::addBus(Bus1PNR * bus)
   {
      switch (bus->type)
      {
         case BusType::SL :
            SLBusses_.push_back(bus);
            break;
         case BusType::PQ : 
            PQBusses_.push_back(bus);
            break;
         case BusType::PV : 
            error() << "PV busses are not supported yet." << std::endl;
            abort();
            break;
      }
   }

   void PowerFlowNR::validate()
   {
      // Determine sizes:
      nSL_ = SLBusses_.size();
      nPQ_ = PQBusses_.size();
      assert(nSL_ == 1); // May change in future...
      assert(nPQ_ > 0); // May change in future...
      nBus_ = nSL_ + nPQ_;
      nTerm_ = 3 * nBus_;
      nVar_ = 6 * nPQ_;
      nVarD2_ = 3 * nPQ_;

      // Size all arrays:
      PPQ_.resize(nTerm_, false);
      QPQ_.resize(nTerm_, false);
      Vr_.resize(nTerm_, false);
      Vi_.resize(nTerm_, false);
      Y_.resize(nTerm_, nTerm_, false);
      G_.resize(nTerm_, nTerm_, false);
      B_.resize(nTerm_, nTerm_, false);
      x_.resize(nVar_, false);
      f_.resize(nVar_, false);
      J_.resize(nVar_, nVar_, false);

      // Insert PQ and PV busses into list of all busses. 
      busses_ = BusVec(); // Clear contents...
      busses_.reserve(nBus_);
      busses_.insert(busses_.end(), PQBusses_.begin(), PQBusses_.end());
      busses_.insert(busses_.end(), SLBusses_.begin(), SLBusses_.end());

      // Index all PQ busses:
      for (int i = 0; i < nPQ_; ++i)
      {
         PQBusses_[i]->idxPQ = i;
      }

      // Set the slack voltages:
      V0_ = SLBusses_[0]->V; // Array copy.

      // Set the PPQ_ and QPQ_ arrays of real and reactive power on each terminal:
      for (int i = 0; i < nPQ_; ++i)
      {
         for (int k = 0; k < 3; ++k)
         {
            PPQ_(3 * i + k) = busses_[i]->P[k];
            QPQ_(3 * i + k) = busses_[i]->Q[k];
         }
      }

      // Build the bus admittance matrix:
      buildBusAdmit();

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);
   }

   void PowerFlowNR::buildBusAdmit()
   {
      int ntot = 3 * (nPQ_ + nSL_);
      for (const Branch1PNR * const branch : branches_)
      {
         const Bus1PNR * busi = branch->busi;
         const Bus1PNR * busk = branch->busk;

         int ibus = busi->idxPQ;
         int kbus = busk->idxPQ;

         int idxi = 3 * ibus;
         int idxk = 3 * kbus;
         for (int p1 = 0; p1 < 3; ++p1)
         {
            for (int p2 = 0; p2 < 3; ++p2)
            {
               Y_(idxi + p1, idxi + p2) += branch->Y[p1][p2];
               Y_(idxk + p1, idxk + p2) += branch->Y[p1 + 3][p2 + 3];
               Y_(idxi + p1, idxk + p2) += branch->Y[p1][p2 + 3];
               Y_(idxk + p1, idxi + p2) += branch->Y[p1 + 3][p2];
            }
         }
      }
   }

   void PowerFlowNR::initx()
   {
      for (int i = 0; i < nPQ_; ++i)
      {
         const Bus1PNR & bus = *busses_[i + 1]; 
         for (int k = 0; k < 3; ++k)
         {
            x_(3 * i + k) = V0_[i].real();
            x_(3 * nPQ_ + 3 * i + k) = V0_[i].imag();
         }
      }
   }

   void PowerFlowNR::updateF()
   {
      using namespace ublas;
      typedef vector_range<vector<double>> VRD; 

      range rPQ(0, 3 * nPQ_);
      range rAll(0, 3 * nPQ_ + 3);
      range r1(0, 3 * nPQ_);
      range r2(3 * nPQ_, 6 * nPQ_);

      VRD x1(x_, r1);
      VRD x2(x_, r2);

      VRD(Vr_, rPQ) = VRD(x_, r1);
      for (int i = 0; i < 3; ++i) Vr_(nPQ_) = V0_.real();
      VRD(Vi, rPQ) = VRD(x_, r2);
      Vi(nPQ_) = V0_.imag();

      ublas::vector<double> M2 = element_prod(Vr, Vr) + element_prod(Vi, Vi); 

      matrix_range<matrix<double>> Grng(G, rPQ, rAll); 
      matrix_range<matrix<double>> Brng(G, rPQ, rAll); 

      ublas::vector<double> DR = element_div((-element_prod(PPQ_, x1) - element_prod(QPQ_, x2)), M2)
                               + prod(Grng, Vr) - prod(Brng, Vi);
      ublas::vector<double> DI = element_div((-element_prod(PPQ_, x2) + element_prod(QPQ_, x1)), M2)
                               + prod(Grng, Vi) + prod(Brng, Vr);

      VRD(f_, r1) = DR;
      VRD(f_, r2) = DI;
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
