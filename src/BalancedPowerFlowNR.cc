#include "BalancedPowerFlowNR.h"
#include "Output.h"

namespace SmartGridToolbox
{
   void BalancedPowerFlowNR::addBus(NRBus * bus)
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
            error("PV busses are not supported yet.");
            break;
      }
   }

   void BalancedPowerFlowNR::validate()
   {
      // Determine sizes:
      nSL_ = SLBusses_.size();
      nPQ_ = PQBusses_.size();
      assert(nSL_ == 1); // May change in future...
      assert(nPQ_ > 0); // May change in future...
      nBus_ = nSL_ + nPQ_;
      nVar_ = 2 * nPQ_;

      // Insert PQ and PV busses into list of all busses.
      // PQ busses at start, slack bus last.
      busses_ = BusVec(); // Clear contents...
      busses_.reserve(nBus_);
      busses_.insert(busses_.end(), PQBusses_.begin(), PQBusses_.end());
      busses_.insert(busses_.end(), SLBusses_.begin(), SLBusses_.end());

      // Set array ranges:
      // Note ublas::range goes from begin (included) to end (excluded).
      rPQ_ = ublas::range(0, nPQ_);
      rAll_ = ublas::range(0, nPQ_ + 1);
      iSL_ = nPQ_;
      rx1_ = ublas::range(0, nPQ_);
      rx2_ = ublas::range(nPQ_, 2 * nPQ_);

      // Size all arrays:
      PPQ_.resize(nPQ_, false);
      QPQ_.resize(nPQ_, false);
      Vr_.resize(nBus_, false);
      Vi_.resize(nBus_, false);
      Y_.resize(nBus_, nBus_, false);
      G_.resize(nBus_, nBus_, false);
      B_.resize(nBus_, nBus_, false);
      x_.resize(nVar_, false);
      f_.resize(nVar_, false);
      J_.resize(nVar_, nVar_, false);

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
         PPQ_(i) = (busses_[i]->S).real();
         QPQ_(i) = (busses_[i]->S).imag();
      }

      // Build the bus admittance matrix:
      buildBusAdmit();

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);
   }

   void BalancedPowerFlowNR::buildBusAdmit()
   {
      for (const NRBranch * const branch : branches_)
      {
         const NRBus * busi = branch->busi;
         const NRBus * busk = branch->busk;

         int ibus = busi->idxPQ;
         int kbus = busk->idxPQ;

         Y_(ibus, ibus) += branch->Y[0][0];
         Y_(kbus, kbus) += branch->Y[1][1];
         Y_(ibus, kbus) += branch->Y[0][1];
         Y_(kbus, ibus) += branch->Y[1][0];
      }
   }

   void BalancedPowerFlowNR::initx()
   {
      for (int i = 0; i < nPQ_; ++i)
      {
         const NRBus & bus = *busses_[i + 1]; 
         x_(i) = V0_.real();
         x_(i + nPQ_) = V0_.imag();
      }
   }

   void BalancedPowerFlowNR::updateF()
   {
      using namespace ublas;
      typedef vector_range<vector<double>> VRD; 

      VRD x1{x_, rx1_};
      VRD x2{x_, rx2_};

      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      VRD(Vr_, rPQ_) = VRD(x_, rx1_);
      VRD(Vi_, rPQ_) = VRD(x_, rx1_);
      Vr_(iSL_) = V0_.real();
      Vi_(iSL_) = V0_.imag();

      ublas::vector<double> M2 = element_prod(Vr_, Vr_) + element_prod(Vi_, Vi_); 

      matrix_range<compressed_matrix<double>> Grng{G_, rPQ_, rAll_};
      matrix_range<compressed_matrix<double>> Brng{G_, rPQ_, rAll_}; 

      ublas::vector<double> dr = element_div((-element_prod(PPQ_, x1) - element_prod(QPQ_, x2)), M2)
                               + prod(Grng, Vr_) - prod(Brng, Vi_);
      ublas::vector<double> di = element_div((-element_prod(PPQ_, x2) + element_prod(QPQ_, x1)), M2)
                               + prod(Grng, Vi_) + prod(Brng, Vr_);

      VRD(f_, rx1_) = dr;
      VRD(f_, rx2_) = di;
   }

   void BalancedPowerFlowNR::solve()
   {
      const double tol = 1e-20;
      const int maxiter = 20;
      initx();
      for (int i = 0; i < maxiter; ++ i)
      {
         updateF();
      }
   }
}
